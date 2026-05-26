import { createServer } from "node:http";
import Database from "better-sqlite3";
import { eq } from "drizzle-orm";
import { drizzle } from "drizzle-orm/better-sqlite3";
import express from "express";
import { Server, type Socket } from "socket.io";
import { documents } from "./schema.ts";

type DocumentPayload = string | { documentId?: string } | undefined;
type TitlePayload = { documentId?: string; title?: string } | undefined;
type BodyPayload = {
  documentId?: string;
  line?: number;
  character?: number;
  text?: string;
  value?: string;
  content?: string;
} | undefined;

const DEFAULT_TITLE = "Documento sin titulo";
const ALLOWED_ORIGINS = [
  "http://localhost:3000",
  "http://localhost:5173",
  "http://127.0.0.1:5173",
];
const PORT = Number(process.env.PORT ?? 3000);

const app = express();
const server = createServer(app);
const io = new Server(server, {
  cors: {
    origin: ALLOWED_ORIGINS,
    methods: ["GET", "POST"],
  },
});

const sqlite = new Database("dev.db");
const db = drizzle(sqlite);

sqlite.exec(`
  CREATE TABLE IF NOT EXISTS Document (
    id TEXT PRIMARY KEY NOT NULL,
    title TEXT NOT NULL,
    content TEXT NOT NULL
  )
`);

function getDocumentId(payload: DocumentPayload) {
  if (typeof payload === "string") {
    return payload.trim();
  }

  return payload?.documentId?.trim() ?? "";
}

function findDocument(documentId: string) {
  return db.select().from(documents).where(eq(documents.id, documentId)).get();
}

function getOrCreateDocument(documentId: string) {
  db.insert(documents)
    .values({ id: documentId, title: DEFAULT_TITLE, content: "" })
    .onConflictDoNothing()
    .run();

  return findDocument(documentId);
}

function updateDocumentTitle(documentId: string, title: string) {
  db.update(documents).set({ title }).where(eq(documents.id, documentId)).run();
}

function updateDocumentContent(documentId: string, content: string) {
  db.update(documents).set({ content }).where(eq(documents.id, documentId)).run();
}

function escapeHtml(value: string) {
  return value
    .replaceAll("&", "&amp;")
    .replaceAll("<", "&lt;")
    .replaceAll(">", "&gt;")
    .replaceAll('"', "&quot;")
    .replaceAll("'", "&#39;");
}

function getVisibleLineOffset(content: string, line: number, character: number) {
  const targetLine = Math.max(Math.trunc(line), 1);
  const targetCharacter = Math.max(Math.trunc(character), 0);
  let currentLine = 1;
  let currentCharacter = 0;
  let insideTag = false;

  for (let index = 0; index < content.length; index += 1) {
    if (currentLine === targetLine && currentCharacter === targetCharacter) {
      return index;
    }

    const current = content[index];
    if (current === "<") {
      insideTag = true;
      continue;
    }

    if (current === ">" && insideTag) {
      insideTag = false;
      continue;
    }

    if (insideTag) {
      continue;
    }

    if (current === "\n") {
      currentLine += 1;
      currentCharacter = 0;
      continue;
    }

    currentCharacter += 1;
  }

  return content.length;
}

function insertTextAtVisiblePosition(content: string, payload: BodyPayload) {
  const insertText = payload?.text ?? payload?.value ?? "";
  const offset = getVisibleLineOffset(
    content,
    payload?.line ?? 1,
    payload?.character ?? 0,
  );

  return content.slice(0, offset) + escapeHtml(insertText) + content.slice(offset);
}

function getNextDocumentContent(content: string, payload: BodyPayload) {
  if (typeof payload?.content === "string") {
    return payload.content;
  }

  return insertTextAtVisiblePosition(content, payload);
}

app.get("/health", (_request, response) => {
  response.json({ status: "ok" });
});

function handleJoinDocument(socket: Socket, data: DocumentPayload) {
  const documentId = getDocumentId(data);
  if (!documentId) {
    return;
  }

  const document = getOrCreateDocument(documentId);
  socket.join(documentId);
  console.log(`Socket ${socket.id} joined document ${documentId}`);
  socket.emit("document:loaded", document);
}

function handleEditTitle(socket: Socket, data: TitlePayload) {
  const documentId = data?.documentId?.trim() ?? "";
  const title = data?.title ?? "";
  if (!documentId) {
    return;
  }

  getOrCreateDocument(documentId);
  updateDocumentTitle(documentId, title);

  io.to(documentId).emit("document:edited:title", {
    documentId,
    title,
    socketId: socket.id,
  });
}

function handleEditBody(socket: Socket, data: BodyPayload) {
  const documentId = data?.documentId?.trim() ?? "";
  if (!documentId) {
    return;
  }

  const document = getOrCreateDocument(documentId);
  if (!document) {
    return;
  }

  const content = getNextDocumentContent(document.content, data);
  updateDocumentContent(documentId, content);

  io.to(documentId).emit("document:edited:body", {
    ...data,
    documentId,
    content,
    socketId: socket.id,
  });
}

io.on("connection", (socket) => {
  console.log(`Socket connected: ${socket.id}`);

  socket.on("disconnect", (reason) => {
    console.log(`Socket disconnected: ${socket.id} (${reason})`);
  });

  socket.on("document:join", (data: DocumentPayload) =>
    handleJoinDocument(socket, data),
  );
  socket.on("document:edit:title", (data: TitlePayload) =>
    handleEditTitle(socket, data),
  );
  socket.on("document:edit:body", (data: BodyPayload) =>
    handleEditBody(socket, data),
  );
});

server.listen(PORT, () => {
  console.log(`Server ready at http://localhost:${PORT}`);
});
