import { createServer } from "node:http";
import Database from "better-sqlite3";
import { eq } from "drizzle-orm";
import { drizzle } from "drizzle-orm/better-sqlite3";
import express from "express";
import { Server, type Socket } from "socket.io";
import { documents } from "./schema.ts";

type DocumentPayload = string | { documentId?: string } | undefined;
type TitlePayload = { documentId?: string; title?: string } | undefined;
type BodyReplaceOp = {
  type?: "replace";
  start?: number;
  deleteCount?: number;
  insertText?: string;
  baseRevision?: number;
  clientMutationId?: string;
} | undefined;
type BodyOpPayload = {
  documentId?: string;
  op?: BodyReplaceOp;
  username?: string;
} | undefined;
type AcceptedBodyReplaceOp = {
  type: "replace";
  start: number;
  deleteCount: number;
  insertText: string;
  baseRevision: number;
  clientMutationId: string;
};
type RevisionedBodyReplaceOp = AcceptedBodyReplaceOp & {
  revision: number;
  socketId: string;
};
type CursorPayload = {
  documentId?: string;
  username?: string;
  selection?: {
    start?: number;
    end?: number;
  };
} | undefined;
type EditorSelection = {
  start: number;
  end: number;
};

const DEFAULT_TITLE = "Documento sin titulo";
const MAX_USERNAME_LENGTH = 32;
const ALLOWED_ORIGINS = new Set([
  "http://localhost:3000",
  "http://localhost:5173",
  "http://127.0.0.1:5173",
  "https://fc7p55gg-3000.brs.devtunnels.ms",
  "https://fc7p55gg-5173.brs.devtunnels.ms",
]);
const PORT = Number(process.env.PORT ?? 3000);
const MAX_BODY_HISTORY_PER_DOCUMENT = 200;
const bodyHistoryByDocument = new Map<string, RevisionedBodyReplaceOp[]>();
const BLOCK_TAGS = new Set([
  "address",
  "article",
  "aside",
  "blockquote",
  "div",
  "figcaption",
  "figure",
  "footer",
  "h1",
  "h2",
  "h3",
  "h4",
  "h5",
  "h6",
  "header",
  "hr",
  "li",
  "main",
  "nav",
  "ol",
  "p",
  "pre",
  "section",
  "ul",
]);

function isAllowedOrigin(origin: string | undefined) {
  if (!origin) {
    return true;
  }

  if (ALLOWED_ORIGINS.has(origin)) {
    return true;
  }

  try {
    const { hostname, protocol } = new URL(origin);
    return protocol === "https:" && hostname.endsWith(".devtunnels.ms");
  } catch {
    return false;
  }
}

const app = express();
const server = createServer(app);
const io = new Server(server, {
  cors: {
    origin(origin, callback) {
      callback(null, isAllowedOrigin(origin));
    },
    methods: ["GET", "POST", "OPTIONS"],
    allowedHeaders: ["Content-Type"],
    credentials: true,
  },
});

const sqlite = new Database("dev.db");
const db = drizzle(sqlite);

app.use((request, response, next) => {
  const origin = request.header("Origin");
  if (origin && !isAllowedOrigin(origin)) {
    response.sendStatus(403);
    return;
  }

  if (origin) {
    response.header("Access-Control-Allow-Origin", origin);
    response.header("Vary", "Origin");
  }
  response.header("Access-Control-Allow-Methods", "GET,POST,OPTIONS");
  response.header("Access-Control-Allow-Headers", "Content-Type");
  response.header("Access-Control-Allow-Credentials", "true");

  if (request.method === "OPTIONS") {
    response.sendStatus(204);
    return;
  }

  next();
});

sqlite.exec(`
  CREATE TABLE IF NOT EXISTS Document (
    id TEXT PRIMARY KEY NOT NULL,
    title TEXT NOT NULL,
    content TEXT NOT NULL,
    revision INTEGER NOT NULL DEFAULT 0
  )
`);

try {
  sqlite.exec("ALTER TABLE Document ADD COLUMN revision INTEGER NOT NULL DEFAULT 0");
} catch (error) {
  if (!(error instanceof Error) || !error.message.includes("duplicate column")) {
    throw error;
  }
}

function getDocumentId(payload: DocumentPayload) {
  if (typeof payload === "string") {
    return payload.trim();
  }

  return payload?.documentId?.trim() ?? "";
}

function sanitizeUsername(value: unknown, socketId: string) {
  if (typeof value !== "string") {
    return `Usuario ${socketId.slice(0, 4)}`;
  }

  const username = value
    .replace(/[\u0000-\u001f\u007f]/g, "")
    .replace(/\s+/g, " ")
    .trim()
    .slice(0, MAX_USERNAME_LENGTH);

  return username || `Usuario ${socketId.slice(0, 4)}`;
}

function findDocument(documentId: string) {
  return db.select().from(documents).where(eq(documents.id, documentId)).get();
}

function getOrCreateDocument(documentId: string) {
  db.insert(documents)
    .values({ id: documentId, title: DEFAULT_TITLE, content: "", revision: 0 })
    .onConflictDoNothing()
    .run();

  return findDocument(documentId);
}

function updateDocumentTitle(documentId: string, title: string) {
  db.update(documents).set({ title }).where(eq(documents.id, documentId)).run();
}

function updateDocumentContent(
  documentId: string,
  content: string,
  revision: number,
) {
  db.update(documents)
    .set({ content, revision })
    .where(eq(documents.id, documentId))
    .run();
}

function escapeHtml(value: string) {
  return value
    .replaceAll("&", "&amp;")
    .replaceAll("<", "&lt;")
    .replaceAll(">", "&gt;")
    .replaceAll('"', "&quot;")
    .replaceAll("'", "&#39;");
}

function decodeHtmlEntity(entity: string) {
  const namedEntities: Record<string, string> = {
    amp: "&",
    lt: "<",
    gt: ">",
    quot: '"',
    apos: "'",
    "#39": "'",
  };

  if (entity in namedEntities) {
    return namedEntities[entity];
  }

  if (entity.startsWith("#x")) {
    const codePoint = Number.parseInt(entity.slice(2), 16);
    return Number.isFinite(codePoint) ? String.fromCodePoint(codePoint) : "&";
  }

  if (entity.startsWith("#")) {
    const codePoint = Number.parseInt(entity.slice(1), 10);
    return Number.isFinite(codePoint) ? String.fromCodePoint(codePoint) : "&";
  }

  return `&${entity};`;
}

function getPlainTextFromHtml(content: string) {
  let text = "";

  for (let index = 0; index < content.length; index += 1) {
    const current = content[index];

    if (current === "<") {
      const tagEnd = content.indexOf(">", index + 1);
      if (tagEnd === -1) {
        text += current;
        continue;
      }

      const tagContent = content.slice(index + 1, tagEnd).trim().toLowerCase();
      const tagName = tagContent
        .replace(/^\//, "")
        .split(/\s+/)[0]
        .replace(/\/$/, "");

      if (tagName === "br" && !text.endsWith("\n")) {
        text += "\n";
      } else if (
        BLOCK_TAGS.has(tagName) &&
        text &&
        !text.endsWith("\n") &&
        tagEnd < content.length - 1
      ) {
        text += "\n";
      }

      index = tagEnd;
      continue;
    }

    if (current === "&") {
      const entityEnd = content.indexOf(";", index + 1);
      if (entityEnd !== -1) {
        text += decodeHtmlEntity(content.slice(index + 1, entityEnd));
        index = entityEnd;
        continue;
      }
    }

    text += current;
  }

  return text;
}

function applyTextOpToPlainText(plainText: string, op: AcceptedBodyReplaceOp) {
  const start = Math.min(op.start, plainText.length);
  const end = Math.min(start + op.deleteCount, plainText.length);

  return {
    content: escapeHtml(
      plainText.slice(0, start) + op.insertText + plainText.slice(end),
    ),
    textLength: plainText.length,
  };
}

function areSelectionsEqual(a: EditorSelection, b: EditorSelection) {
  return a.start === b.start && a.end === b.end;
}

function transformPosition(
  position: number,
  appliedOp: RevisionedBodyReplaceOp,
  affinity: "left" | "right",
) {
  const appliedStart = appliedOp.start;
  const appliedEnd = appliedStart + appliedOp.deleteCount;
  const delta = appliedOp.insertText.length - appliedOp.deleteCount;

  if (position < appliedStart) {
    return position;
  }

  if (position > appliedEnd) {
    return Math.max(0, position + delta);
  }

  if (position === appliedStart) {
    return affinity === "right"
      ? appliedStart + appliedOp.insertText.length
      : appliedStart;
  }

  if (position === appliedEnd) {
    return appliedStart + appliedOp.insertText.length;
  }

  return appliedStart + appliedOp.insertText.length;
}

function transformBodyOp(
  op: AcceptedBodyReplaceOp,
  appliedOps: RevisionedBodyReplaceOp[],
) {
  let start = op.start;
  let end = op.start + op.deleteCount;

  for (const appliedOp of appliedOps) {
    start = transformPosition(start, appliedOp, "right");
    end = transformPosition(end, appliedOp, "right");
    if (end < start) {
      end = start;
    }
  }

  return {
    ...op,
    start,
    deleteCount: Math.max(0, end - start),
  };
}

function normalizeBodyOp(payload: BodyOpPayload): AcceptedBodyReplaceOp | null {
  const op = payload?.op;
  const start = Number(op?.start);
  const deleteCount = Number(op?.deleteCount);
  const baseRevision = Number(op?.baseRevision);

  if (
    op?.type !== "replace" ||
    !Number.isFinite(start) ||
    !Number.isFinite(deleteCount) ||
    !Number.isFinite(baseRevision) ||
    typeof op.insertText !== "string" ||
    typeof op.clientMutationId !== "string" ||
    !op.clientMutationId
  ) {
    return null;
  }

  return {
    type: "replace",
    start: Math.max(0, Math.trunc(start)),
    deleteCount: Math.max(0, Math.trunc(deleteCount)),
    insertText: op.insertText,
    baseRevision: Math.max(0, Math.trunc(baseRevision)),
    clientMutationId: op.clientMutationId,
  };
}

function appendBodyHistory(documentId: string, op: RevisionedBodyReplaceOp) {
  const history = bodyHistoryByDocument.get(documentId) ?? [];
  history.push(op);
  if (history.length > MAX_BODY_HISTORY_PER_DOCUMENT) {
    history.splice(0, history.length - MAX_BODY_HISTORY_PER_DOCUMENT);
  }
  bodyHistoryByDocument.set(documentId, history);
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
  socket.data.documentId = documentId;
  socket.join(documentId);
  console.log(`Socket ${socket.id} joined document ${documentId}`);
  socket.emit("document:loaded", document);
}

function handleCursorUpdate(socket: Socket, data: CursorPayload) {
  const documentId = data?.documentId?.trim() ?? "";
  const selection = data?.selection;
  if (!documentId || !selection) {
    return;
  }

  const start = Number(selection.start);
  const end = Number(selection.end);
  if (!Number.isFinite(start) || !Number.isFinite(end)) {
    return;
  }

  const nextSelection = {
    start: Math.max(0, Math.trunc(start)),
    end: Math.max(0, Math.trunc(end)),
  };
  const username = sanitizeUsername(data?.username, socket.id);
  if (
    socket.data.username === username &&
    socket.data.selection &&
    areSelectionsEqual(socket.data.selection, nextSelection)
  ) {
    return;
  }

  socket.data.documentId = documentId;
  socket.data.username = username;
  socket.data.selection = nextSelection;
  socket.to(documentId).emit("document:cursor", {
    documentId,
    socketId: socket.id,
    username: socket.data.username,
    selection: nextSelection,
  });
}

function handleEditTitle(socket: Socket, data: TitlePayload) {
  const documentId = data?.documentId?.trim() ?? "";
  const title = data?.title ?? "";
  if (!documentId) {
    return;
  }

  getOrCreateDocument(documentId);
  const currentDocument = findDocument(documentId);
  if (currentDocument?.title === title) {
    return;
  }

  updateDocumentTitle(documentId, title);

  io.to(documentId).emit("document:edited:title", {
    documentId,
    title,
    socketId: socket.id,
  });
}

function emitBodyResync(socket: Socket, documentId: string, content: string, revision: number) {
  socket.emit("document:body:resync", {
    documentId,
    content,
    revision,
  });
}

function handleEditBodyOp(socket: Socket, data: BodyOpPayload) {
  const documentId = data?.documentId?.trim() ?? "";
  if (!documentId) {
    return;
  }

  const document = getOrCreateDocument(documentId);
  const normalizedOp = normalizeBodyOp(data);
  if (!document || !normalizedOp) {
    return;
  }

  const history = bodyHistoryByDocument.get(documentId) ?? [];
  const earliestRevisionInHistory = history[0]?.revision ?? document.revision;
  if (
    normalizedOp.baseRevision > document.revision ||
    (normalizedOp.baseRevision < document.revision &&
      normalizedOp.baseRevision < earliestRevisionInHistory - 1)
  ) {
    emitBodyResync(socket, documentId, document.content, document.revision);
    return;
  }

  const opsAfterBaseRevision = history.filter(
    (historyOp) => historyOp.revision > normalizedOp.baseRevision,
  );
  const transformedOp = transformBodyOp(normalizedOp, opsAfterBaseRevision);
  const plainText = getPlainTextFromHtml(document.content);
  if (transformedOp.start > plainText.length) {
    emitBodyResync(socket, documentId, document.content, document.revision);
    return;
  }

  const safeTransformedOp = {
    ...transformedOp,
    deleteCount: Math.min(
      transformedOp.deleteCount,
      plainText.length - transformedOp.start,
    ),
  };
  const { content } = applyTextOpToPlainText(plainText, safeTransformedOp);
  const revision = document.revision + 1;
  const acceptedOp: RevisionedBodyReplaceOp = {
    ...safeTransformedOp,
    revision,
    socketId: socket.id,
  };

  updateDocumentContent(documentId, content, revision);
  appendBodyHistory(documentId, acceptedOp);

  io.to(documentId).emit("document:edited:body:op", {
    documentId,
    op: safeTransformedOp,
    content,
    revision,
    socketId: socket.id,
    username: sanitizeUsername(data?.username, socket.id),
  });
}

io.on("connection", (socket) => {
  console.log(`Socket connected: ${socket.id}`);

  socket.on("disconnect", (reason) => {
    console.log(`Socket disconnected: ${socket.id} (${reason})`);

    const documentId = socket.data.documentId;
    if (typeof documentId === "string" && documentId) {
      socket.to(documentId).emit("document:cursor:removed", {
        documentId,
        socketId: socket.id,
      });
    }
  });

  socket.on("document:join", (data: DocumentPayload) =>
    handleJoinDocument(socket, data),
  );
  socket.on("document:edit:title", (data: TitlePayload) =>
    handleEditTitle(socket, data),
  );
  socket.on("document:edit:body:op", (data: BodyOpPayload) =>
    handleEditBodyOp(socket, data),
  );
  socket.on("document:cursor", (data: CursorPayload) =>
    handleCursorUpdate(socket, data),
  );
});

server.listen(PORT, () => {
  console.log(`Server ready at http://localhost:${PORT}`);
});
