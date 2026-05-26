import { type MouseEvent, useEffect, useRef, useState } from "react";
import { socket } from "./socket.ts";
import "./App.css";

const DEFAULT_TITLE = "Documento sin titulo";
const BODY_SAVE_DELAY_MS = 500;

type ActiveMarks = {
  bold: boolean;
  italic: boolean;
  underline: boolean;
  strikeThrough: boolean;
};

type FormatCommand = keyof ActiveMarks;

type DocumentState = {
  id: string;
  title: string;
  content: string;
};

const DEFAULT_ACTIVE_MARKS: ActiveMarks = {
  bold: false,
  italic: false,
  underline: false,
  strikeThrough: false,
};

const TOOLBAR_BUTTONS: Array<{ command: FormatCommand; label: string }> = [
  { command: "bold", label: "B" },
  { command: "italic", label: "I" },
  { command: "underline", label: "U" },
  { command: "strikeThrough", label: "S" },
];

const FONT_SIZE_OPTIONS = [
  { value: "2", label: "14 px" },
  { value: "3", label: "16 px" },
  { value: "4", label: "18 px" },
  { value: "5", label: "24 px" },
  { value: "6", label: "32 px" },
];

function getInitialDocumentId() {
  const documentId = decodeURIComponent(
    window.location.pathname.slice(1),
  ).trim();
  if (documentId) {
    return documentId;
  }

  const nextDocumentId = crypto.randomUUID();
  window.history.replaceState({}, "", `/${nextDocumentId}`);
  return nextDocumentId;
}

function commandState(command: string) {
  try {
    return window.document.queryCommandState(command);
  } catch {
    return false;
  }
}

function preventEditorBlur(event: MouseEvent<HTMLButtonElement>) {
  event.preventDefault();
}

function App() {
  const [socketConnected, setSocketConnected] = useState(false);
  const [activeDocumentId] = useState(getInitialDocumentId);
  const [document, setDocument] = useState<DocumentState | null>(null);
  const [titleDraft, setTitleDraft] = useState("");
  const [activeMarks, setActiveMarks] = useState(DEFAULT_ACTIVE_MARKS);

  const editorRef = useRef<HTMLDivElement | null>(null);
  const pendingContentRef = useRef<string | null>(null);
  const pendingDocumentIdRef = useRef<string | null>(null);
  const sendTimerRef = useRef<number | null>(null);

  const flushPendingBodySave = () => {
    if (sendTimerRef.current !== null) {
      window.clearTimeout(sendTimerRef.current);
      sendTimerRef.current = null;
    }

    const content = pendingContentRef.current;
    const documentId = pendingDocumentIdRef.current;
    pendingContentRef.current = null;
    pendingDocumentIdRef.current = null;

    if (content === null || documentId === null) {
      return;
    }

    socket.emit("document:edit:body", { documentId, content });
  };

  const scheduleBodySave = (documentId: string, content: string) => {
    pendingContentRef.current = content;
    pendingDocumentIdRef.current = documentId;

    if (sendTimerRef.current === null) {
      sendTimerRef.current = window.setTimeout(
        flushPendingBodySave,
        BODY_SAVE_DELAY_MS,
      );
    }
  };

  useEffect(() => {
    const clearPendingBodySave = () => {
      if (sendTimerRef.current !== null) {
        window.clearTimeout(sendTimerRef.current);
        sendTimerRef.current = null;
      }

      pendingContentRef.current = null;
      pendingDocumentIdRef.current = null;
    };

    const handleConnect = () => {
      setSocketConnected(true);
      socket.emit("document:join", { documentId: activeDocumentId });
    };

    const handleDisconnect = () => setSocketConnected(false);
    const handleLoaded = (data: DocumentState) => {
      setDocument(data);
      setTitleDraft(data.title);
      if (editorRef.current) {
        editorRef.current.innerHTML = data.content;
      }
    };
    const handleTitleEdited = (data: {
      documentId: string;
      title: string;
      socketId?: string;
    }) => {
      const isOwnEvent = data.socketId === socket.id;

      setDocument((current) =>
        current?.id === data.documentId
          ? { ...current, title: data.title }
          : current,
      );
      if (data.documentId === activeDocumentId) {
        setTitleDraft((current) =>
          isOwnEvent && current === data.title ? current : data.title,
        );
      }
    };
    const handleBodyEdited = (data: {
      documentId: string;
      content: string;
      socketId?: string;
    }) => {
      if (data.socketId === socket.id) {
        return;
      }

      clearPendingBodySave();

      setDocument((current) => {
        if (current?.id !== data.documentId) {
          return current;
        }

        if (editorRef.current && editorRef.current.innerHTML !== data.content) {
          editorRef.current.innerHTML = data.content;
        }

        return { ...current, content: data.content };
      });
    };

    socket.on("connect", handleConnect);
    socket.on("disconnect", handleDisconnect);
    socket.on("document:loaded", handleLoaded);
    socket.on("document:edited:title", handleTitleEdited);
    socket.on("document:edited:body", handleBodyEdited);

    if (socket.connected) {
      handleConnect();
    }

    return () => {
      clearPendingBodySave();
      socket.off("connect", handleConnect);
      socket.off("disconnect", handleDisconnect);
      socket.off("document:loaded", handleLoaded);
      socket.off("document:edited:title", handleTitleEdited);
      socket.off("document:edited:body", handleBodyEdited);
    };
  }, [activeDocumentId]);

  const saveTitle = () => {
    if (!document) {
      return;
    }

    const title = titleDraft.trim() ? titleDraft : DEFAULT_TITLE;
    if (title === document.title) {
      return;
    }

    setDocument({ ...document, title });
    socket.emit("document:edit:title", { documentId: document.id, title });
  };

  const handleBodyInput = () => {
    if (!document || !editorRef.current) {
      return;
    }

    const content = editorRef.current.innerHTML;
    setDocument({ ...document, content });
    scheduleBodySave(document.id, content);
  };

  const updateActiveFormats = () => {
    setActiveMarks({
      bold: commandState("bold"),
      italic: commandState("italic"),
      underline: commandState("underline"),
      strikeThrough: commandState("strikeThrough"),
    });
  };

  const applyFormatCommand = (command: string, value?: string) => {
    editorRef.current?.focus();
    window.document.execCommand(command, false, value);
    handleBodyInput();
    updateActiveFormats();
  };

  return (
    <main className="app">
      <header className="topbar">
        <div className="topbar-content">
          <input
            className="title"
            value={titleDraft}
            onBlur={saveTitle}
            onChange={(event) => setTitleDraft(event.target.value)}
            onKeyDown={(event) => {
              if (event.key === "Enter") {
                event.preventDefault();
                event.currentTarget.blur();
              }
            }}
            placeholder="Titulo"
            disabled={!document}
          />
          <div className="toolbar" aria-label="Formato del documento">
            {TOOLBAR_BUTTONS.map(({ command, label }) => (
              <button
                className={activeMarks[command] ? "active" : ""}
                disabled={!document}
                key={command}
                onMouseDown={preventEditorBlur}
                onClick={() => applyFormatCommand(command)}
                type="button"
              >
                {label}
              </button>
            ))}
            <select
              defaultValue="3"
              disabled={!document}
              onChange={(event) =>
                applyFormatCommand("fontSize", event.target.value)
              }
            >
              {FONT_SIZE_OPTIONS.map(({ value, label }) => (
                <option key={value} value={value}>
                  {label}
                </option>
              ))}
            </select>
            <span>{socketConnected ? "Conectado" : "Desconectado"}</span>
          </div>
        </div>
      </header>

      <section className="editor">
        <div
          ref={editorRef}
          className="document-body"
          contentEditable={Boolean(document)}
          onInput={handleBodyInput}
          onKeyUp={updateActiveFormats}
          onMouseUp={updateActiveFormats}
          role="textbox"
          aria-label="Contenido del documento"
          data-placeholder="Escribe el contenido del documento"
          suppressContentEditableWarning
        />
      </section>
    </main>
  );
}

export default App;
