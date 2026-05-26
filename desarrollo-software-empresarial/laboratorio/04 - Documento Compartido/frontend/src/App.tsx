import {
  type CSSProperties,
  useEffect,
  useEffectEvent,
  useRef,
  useState,
} from "react";
import { socket } from "./socket.ts";
import "./App.css";

const DEFAULT_TITLE = "Documento sin titulo";
const BODY_SAVE_DELAY_MS = 35;
const CURSOR_SEND_THROTTLE_MS = 80;
const DEFAULT_USERNAME_PREFIX = "Usuario";
const ELEMENTOS_BLOQUE_EDITOR = new Set([
  "ADDRESS",
  "ARTICLE",
  "ASIDE",
  "BLOCKQUOTE",
  "DIV",
  "FIGCAPTION",
  "FIGURE",
  "FOOTER",
  "H1",
  "H2",
  "H3",
  "H4",
  "H5",
  "H6",
  "HEADER",
  "HR",
  "LI",
  "MAIN",
  "NAV",
  "OL",
  "P",
  "PRE",
  "SECTION",
  "UL",
]);

type DocumentState = {
  id: string;
  title: string;
  content: string;
  revision: number;
};

type BodyReplaceOp = {
  type: "replace";
  start: number;
  deleteCount: number;
  insertText: string;
  baseRevision: number;
  clientMutationId: string;
};

type QueuedBodyReplaceOp = Omit<
  BodyReplaceOp,
  "baseRevision" | "clientMutationId"
>;

type BodyOpEditedEvent = {
  documentId: string;
  op: BodyReplaceOp;
  content: string;
  revision: number;
  socketId?: string;
  clientMutationId?: string;
};

type BodyResyncEvent = {
  documentId: string;
  content: string;
  revision: number;
};

type EditorSelection = {
  start: number;
  end: number;
};

type RemoteCursor = {
  socketId: string;
  username: string;
  selection: EditorSelection;
};

type CursorOverlay = RemoteCursor & {
  color: string;
  left: number;
  top: number;
  height: number;
};

function obtenerIdDocumentoInicial() {
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

function obtenerNombreUsuarioAleatorio() {
  return `${DEFAULT_USERNAME_PREFIX} ${Math.floor(1000 + Math.random() * 9000)}`;
}

function obtenerDesplazamientoTexto(editor: HTMLElement, node: Node, offset: number) {
  const currentRange = window.document.createRange();
  currentRange.selectNodeContents(editor);
  currentRange.setEnd(node, offset);
  const fragment = currentRange.cloneContents();
  return obtenerTextoPlanoDesdeNodo(fragment).length;
}

function obtenerTextoPlanoDesdeHtml(html: string) {
  const container = window.document.createElement("div");
  container.innerHTML = html;
  return obtenerTextoPlanoDelEditor(container);
}

function obtenerTextoPlanoDesdeNodo(node: Node): string {
  if (node.nodeType === Node.TEXT_NODE) {
    return node.textContent ?? "";
  }

  if (node.nodeType !== Node.ELEMENT_NODE && node.nodeType !== Node.DOCUMENT_FRAGMENT_NODE) {
    return "";
  }

  if (node instanceof HTMLBRElement) {
    return "\n";
  }

  const isBlock =
    node instanceof HTMLElement && ELEMENTOS_BLOQUE_EDITOR.has(node.tagName);
  const childText = obtenerTextoPlanoDesdeHijos(Array.from(node.childNodes));

  if (isBlock && childText === "\n") {
    return "";
  }

  return childText;
}

function obtenerTextoPlanoDesdeHijos(children: Node[]) {
  let text = "";

  children.forEach((child, index) => {
    const isBlock =
      child instanceof HTMLElement && ELEMENTOS_BLOQUE_EDITOR.has(child.tagName);

    if (isBlock && text && !text.endsWith("\n")) {
      text += "\n";
    }

    text += obtenerTextoPlanoDesdeNodo(child);

    if (isBlock && index < children.length - 1 && !text.endsWith("\n")) {
      text += "\n";
    }
  });

  return text;
}

function obtenerTextoPlanoDelEditor(editor: HTMLElement) {
  return obtenerTextoPlanoDesdeHijos(Array.from(editor.childNodes));
}

function crearOperacionReemplazoDesdeCambioTexto(
  previousText: string,
  nextText: string,
): QueuedBodyReplaceOp | null {
  if (previousText === nextText) {
    return null;
  }

  let prefixLength = 0;
  const shortestLength = Math.min(previousText.length, nextText.length);
  while (
    prefixLength < shortestLength &&
    previousText[prefixLength] === nextText[prefixLength]
  ) {
    prefixLength += 1;
  }

  let suffixLength = 0;
  while (
    suffixLength < previousText.length - prefixLength &&
    suffixLength < nextText.length - prefixLength &&
    previousText[previousText.length - 1 - suffixLength] ===
    nextText[nextText.length - 1 - suffixLength]
  ) {
    suffixLength += 1;
  }

  return {
    type: "replace",
    start: prefixLength,
    deleteCount: previousText.length - prefixLength - suffixLength,
    insertText: nextText.slice(prefixLength, nextText.length - suffixLength),
  };
}

function transformarPosicionPorOperacion(
  position: number,
  op: Pick<BodyReplaceOp, "start" | "deleteCount" | "insertText">,
  affinity: "left" | "right",
) {
  const opEnd = op.start + op.deleteCount;
  const delta = op.insertText.length - op.deleteCount;

  if (position < op.start) {
    return position;
  }

  if (position > opEnd) {
    return Math.max(0, position + delta);
  }

  if (position === op.start) {
    return affinity === "right" ? op.start + op.insertText.length : op.start;
  }

  if (position === opEnd) {
    return op.start + op.insertText.length;
  }

  return op.start + op.insertText.length;
}

function transformarSeleccionPorOperacion(
  selection: EditorSelection,
  op: Pick<BodyReplaceOp, "start" | "deleteCount" | "insertText">,
): EditorSelection {
  return {
    start: transformarPosicionPorOperacion(selection.start, op, "right"),
    end: transformarPosicionPorOperacion(selection.end, op, "right"),
  };
}

function transformarOperacionPendientePorOperacion(
  queuedOp: QueuedBodyReplaceOp,
  appliedOp: Pick<BodyReplaceOp, "start" | "deleteCount" | "insertText">,
): QueuedBodyReplaceOp {
  const start = transformarPosicionPorOperacion(queuedOp.start, appliedOp, "right");
  const end = transformarPosicionPorOperacion(
    queuedOp.start + queuedOp.deleteCount,
    appliedOp,
    "right",
  );

  return {
    ...queuedOp,
    start,
    deleteCount: Math.max(0, end - start),
  };
}

function obtenerSeleccionEditor(editor: HTMLElement): EditorSelection | null {
  const selection = window.getSelection();
  if (!selection || selection.rangeCount === 0) {
    return null;
  }

  const range = selection.getRangeAt(0);
  if (
    !editor.contains(range.startContainer) ||
    !editor.contains(range.endContainer)
  ) {
    return null;
  }

  return {
    start: obtenerDesplazamientoTexto(editor, range.startContainer, range.startOffset),
    end: obtenerDesplazamientoTexto(editor, range.endContainer, range.endOffset),
  };
}

function obtenerPosicionTexto(editor: HTMLElement, offset: number) {
  const targetOffset = Math.max(0, offset);
  const walker = window.document.createTreeWalker(editor, NodeFilter.SHOW_TEXT);
  let currentOffset = 0;
  let currentNode = walker.nextNode();

  while (currentNode) {
    const textLength = currentNode.textContent?.length ?? 0;
    const nextOffset = currentOffset + textLength;

    if (targetOffset <= nextOffset) {
      return {
        node: currentNode,
        offset: targetOffset - currentOffset,
      };
    }

    currentOffset = nextOffset;
    currentNode = walker.nextNode();
  }

  return {
    node: editor,
    offset: editor.childNodes.length,
  };
}

function restaurarSeleccionEditor(editor: HTMLElement, selection: EditorSelection) {
  const browserSelection = window.getSelection();
  if (!browserSelection) {
    return;
  }

  const range = window.document.createRange();
  const start = obtenerPosicionTexto(editor, selection.start);
  const end = obtenerPosicionTexto(editor, selection.end);

  range.setStart(start.node, start.offset);
  range.setEnd(end.node, end.offset);
  browserSelection.removeAllRanges();
  browserSelection.addRange(range);
}

function aplicarOperacionReemplazoAlEditor(editor: HTMLElement, op: BodyReplaceOp) {
  const range = window.document.createRange();
  const start = obtenerPosicionTexto(editor, op.start);
  const end = obtenerPosicionTexto(editor, op.start + op.deleteCount);

  range.setStart(start.node, start.offset);
  range.setEnd(end.node, end.offset);
  range.deleteContents();
  if (op.insertText) {
    range.insertNode(window.document.createTextNode(op.insertText));
  }
}

function insertarTextoEnSeleccionEditor(editor: HTMLElement, text: string) {
  const selection = window.getSelection();
  if (!selection || selection.rangeCount === 0) {
    return false;
  }

  const range = selection.getRangeAt(0);
  if (
    !editor.contains(range.startContainer) ||
    !editor.contains(range.endContainer)
  ) {
    return false;
  }

  range.deleteContents();
  const textNode = window.document.createTextNode(text);
  range.insertNode(textNode);
  range.setStart(textNode, text.length);
  range.collapse(true);
  selection.removeAllRanges();
  selection.addRange(range);
  editor.dispatchEvent(
    new InputEvent("input", {
      bubbles: true,
      data: text,
      inputType: "insertText",
    }),
  );

  return true;
}

function obtenerColorSocket(socketId: string) {
  let hash = 0;
  for (let index = 0; index < socketId.length; index += 1) {
    hash = (hash * 31 + socketId.charCodeAt(index)) % 360;
  }

  return `hsl(${hash} 75% 45%)`;
}

function obtenerEtiquetaCursor(cursor: RemoteCursor) {
  return cursor.username.trim() || cursor.socketId.slice(0, 4);
}

function obtenerRectanguloCursor(editor: HTMLElement, offset: number) {
  const textPosition = obtenerPosicionTexto(editor, offset);
  const range = window.document.createRange();
  range.setStart(textPosition.node, textPosition.offset);
  range.collapse(true);

  const rect = range.getBoundingClientRect();
  if (rect.width || rect.height) {
    return rect;
  }

  return range.getClientRects()[0] ?? null;
}

function transformarCursoresRemotosPorOperacion(
  cursors: RemoteCursor[],
  op: Pick<BodyReplaceOp, "start" | "deleteCount" | "insertText">,
) {
  return cursors.map((cursor) => ({
    ...cursor,
    selection: transformarSeleccionPorOperacion(cursor.selection, op),
  }));
}

function sonSeleccionesIguales(a: EditorSelection, b: EditorSelection) {
  return a.start === b.start && a.end === b.end;
}

function sonSuperposicionesCursoresIguales(
  a: CursorOverlay[],
  b: CursorOverlay[],
) {
  return (
    a.length === b.length &&
    a.every((overlay, index) => {
      const other = b[index];
      return (
        other &&
        overlay.socketId === other.socketId &&
        overlay.username === other.username &&
        overlay.color === other.color &&
        overlay.left === other.left &&
        overlay.top === other.top &&
        overlay.height === other.height &&
        sonSeleccionesIguales(overlay.selection, other.selection)
      );
    })
  );
}

function App() {
  const [socketConnected, setSocketConnected] = useState(false);
  const [activeDocumentId] = useState(obtenerIdDocumentoInicial);
  const [document, setDocument] = useState<DocumentState | null>(null);
  const [titleDraft, setTitleDraft] = useState("");
  const [username, setUsername] = useState(obtenerNombreUsuarioAleatorio);
  const [remoteCursors, setRemoteCursors] = useState<RemoteCursor[]>([]);
  const [cursorOverlays, setCursorOverlays] = useState<CursorOverlay[]>([]);

  const editorWrapperRef = useRef<HTMLElement | null>(null);
  const editorRef = useRef<HTMLDivElement | null>(null);
  const queuedOpsRef = useRef<QueuedBodyReplaceOp[]>([]);
  const queuedDocumentIdRef = useRef<string | null>(null);
  const sendTimerRef = useRef<number | null>(null);
  const appliedRevisionRef = useRef(0);
  const mutationCounterRef = useRef(0);
  const inFlightMutationRef = useRef<{
    documentId: string;
    op: BodyReplaceOp;
    clientMutationId: string;
  } | null>(null);
  const lastKnownBodyTextRef = useRef("");
  const documentRef = useRef<DocumentState | null>(null);
  const remoteCursorsRef = useRef<RemoteCursor[]>([]);
  const cursorOverlayFrameRef = useRef<number | null>(null);
  const lastCursorOverlaysRef = useRef<CursorOverlay[]>([]);
  const lastCursorEmitRef = useRef(0);
  const pendingCursorEmitTimerRef = useRef<number | null>(null);
  const lastLocalSelectionRef = useRef<EditorSelection | null>(null);
  const lastEmittedSelectionRef = useRef<EditorSelection | null>(null);
  const usernameRef = useRef(username);

  const actualizarSuperposicionesCursores = () => {
    const editor = editorRef.current;
    const wrapper = editorWrapperRef.current;
    if (!editor || !wrapper) {
      setCursorOverlays([]);
      return;
    }

    const wrapperRect = wrapper.getBoundingClientRect();
    const nextOverlays = remoteCursorsRef.current.flatMap((cursor) => {
      const rect = obtenerRectanguloCursor(editor, cursor.selection.end);
      if (!rect) {
        const previousOverlay = lastCursorOverlaysRef.current.find(
          (overlay) => overlay.socketId === cursor.socketId,
        );
        return previousOverlay ? [{ ...previousOverlay, ...cursor }] : [];
      }

      return [
        {
          ...cursor,
          color: obtenerColorSocket(cursor.socketId),
          left: rect.left - wrapperRect.left,
          top: rect.top - wrapperRect.top,
          height: rect.height || 22,
        },
      ];
    });

    if (!sonSuperposicionesCursoresIguales(lastCursorOverlaysRef.current, nextOverlays)) {
      lastCursorOverlaysRef.current = nextOverlays;
      setCursorOverlays(nextOverlays);
    }
  };

  const programarActualizacionSuperposicionesCursores = () => {
    if (cursorOverlayFrameRef.current !== null) {
      return;
    }

    cursorOverlayFrameRef.current = window.requestAnimationFrame(() => {
      cursorOverlayFrameRef.current = null;
      actualizarSuperposicionesCursores();
    });
  };

  const emitirSeleccionCursor = () => {
    const currentDocument = documentRef.current;
    const editor = editorRef.current;
    if (!currentDocument || !editor) {
      return;
    }

    const selection = obtenerSeleccionEditor(editor) ?? lastLocalSelectionRef.current;
    if (!selection) {
      return;
    }

    lastLocalSelectionRef.current = selection;

    if (
      lastEmittedSelectionRef.current &&
      sonSeleccionesIguales(lastEmittedSelectionRef.current, selection)
    ) {
      return;
    }

    lastEmittedSelectionRef.current = selection;

    socket.emit("document:cursor", {
      documentId: currentDocument.id,
      username: usernameRef.current,
      selection,
    });
  };

  const programarEmisionSeleccionCursor = () => {
    const elapsed = Date.now() - lastCursorEmitRef.current;
    if (elapsed >= CURSOR_SEND_THROTTLE_MS) {
      lastCursorEmitRef.current = Date.now();
      emitirSeleccionCursor();
      return;
    }

    if (pendingCursorEmitTimerRef.current !== null) {
      return;
    }

    pendingCursorEmitTimerRef.current = window.setTimeout(() => {
      pendingCursorEmitTimerRef.current = null;
      lastCursorEmitRef.current = Date.now();
      emitirSeleccionCursor();
    }, CURSOR_SEND_THROTTLE_MS - elapsed);
  };

  const enviarOperacionCuerpoPendiente = () => {
    if (sendTimerRef.current !== null) {
      window.clearTimeout(sendTimerRef.current);
      sendTimerRef.current = null;
    }

    if (inFlightMutationRef.current !== null) {
      return;
    }

    const queuedOp = queuedOpsRef.current.shift() ?? null;
    const documentId = queuedDocumentIdRef.current;
    if (queuedOpsRef.current.length === 0) {
      queuedDocumentIdRef.current = null;
    }

    if (queuedOp === null || documentId === null) {
      return;
    }

    const clientMutationId = `${socket.id ?? "pending"}:${mutationCounterRef.current + 1
      }`;
    mutationCounterRef.current += 1;
    const op: BodyReplaceOp = {
      ...queuedOp,
      baseRevision: appliedRevisionRef.current,
      clientMutationId,
    };
    inFlightMutationRef.current = { documentId, op, clientMutationId };
    socket.emit("document:edit:body:op", {
      documentId,
      op,
      username: usernameRef.current,
    });
  };

  const programarGuardadoCuerpo = (documentId: string, op: QueuedBodyReplaceOp) => {
    queuedOpsRef.current.push(op);
    queuedDocumentIdRef.current = documentId;

    if (inFlightMutationRef.current !== null) {
      return;
    }

    if (sendTimerRef.current === null) {
      sendTimerRef.current = window.setTimeout(
        enviarOperacionCuerpoPendiente,
        BODY_SAVE_DELAY_MS,
      );
    }
  };

  const programarActualizacionSuperposicionesCursoresDesdeEfecto =
    useEffectEvent(programarActualizacionSuperposicionesCursores);
  const programarEmisionSeleccionCursorDesdeEfecto = useEffectEvent(
    programarEmisionSeleccionCursor,
  );

  useEffect(() => {
    documentRef.current = document;
    if (document) {
      appliedRevisionRef.current = document.revision;
    }
  }, [document]);

  useEffect(() => {
    usernameRef.current = username;
  }, [username]);

  useEffect(() => {
    remoteCursorsRef.current = remoteCursors;
    programarActualizacionSuperposicionesCursoresDesdeEfecto();
  }, [remoteCursors]);

  useEffect(() => {
    const manejarCambioSeleccion = () => programarEmisionSeleccionCursorDesdeEfecto();
    const manejarCambioVista = () =>
      programarActualizacionSuperposicionesCursoresDesdeEfecto();

    window.document.addEventListener("selectionchange", manejarCambioSeleccion);
    window.addEventListener("resize", manejarCambioVista);
    window.addEventListener("scroll", manejarCambioVista, true);

    return () => {
      window.document.removeEventListener(
        "selectionchange",
        manejarCambioSeleccion,
      );
      window.removeEventListener("resize", manejarCambioVista);
      window.removeEventListener("scroll", manejarCambioVista, true);
      if (cursorOverlayFrameRef.current !== null) {
        window.cancelAnimationFrame(cursorOverlayFrameRef.current);
        cursorOverlayFrameRef.current = null;
      }
      if (pendingCursorEmitTimerRef.current !== null) {
        window.clearTimeout(pendingCursorEmitTimerRef.current);
        pendingCursorEmitTimerRef.current = null;
      }
    };
  }, []);

  useEffect(() => {
    const limpiarGuardadoCuerpoPendiente = () => {
      if (sendTimerRef.current !== null) {
        window.clearTimeout(sendTimerRef.current);
        sendTimerRef.current = null;
      }

      queuedOpsRef.current = [];
      queuedDocumentIdRef.current = null;
      inFlightMutationRef.current = null;
    };

    const manejarConexion = () => {
      setSocketConnected(true);
      socket.emit("document:join", { documentId: activeDocumentId });
    };

    const manejarDesconexion = () => setSocketConnected(false);
    const manejarDocumentoCargado = (data: DocumentState) => {
      const loadedDocument = { ...data, revision: data.revision ?? 0 };
      appliedRevisionRef.current = loadedDocument.revision;
      setDocument(loadedDocument);
      setTitleDraft(data.title);
      setRemoteCursors([]);
      lastCursorOverlaysRef.current = [];
      if (editorRef.current) {
        editorRef.current.textContent = obtenerTextoPlanoDesdeHtml(data.content);
        lastKnownBodyTextRef.current = obtenerTextoPlanoDelEditor(editorRef.current);
        programarActualizacionSuperposicionesCursoresDesdeEfecto();
      }
    };
    const manejarTituloEditado = (data: {
      documentId: string;
      title: string;
      socketId?: string;
    }) => {
      const isOwnEvent = data.socketId === socket.id;

      setDocument((current) => {
        if (current?.id !== data.documentId) {
          return current;
        }

        const nextDocument = {
          ...(documentRef.current ?? current),
          title: data.title,
        };
        documentRef.current = nextDocument;
        return nextDocument;
      });
      if (data.documentId === activeDocumentId) {
        setTitleDraft((current) =>
          isOwnEvent && current === data.title ? current : data.title,
        );
      }
    };
    const manejarOperacionCuerpoEditada = (data: BodyOpEditedEvent) => {
      if (data.documentId !== activeDocumentId) {
        return;
      }

      if (data.revision <= appliedRevisionRef.current) {
        return;
      }

      const matchingInFlightMutation =
        inFlightMutationRef.current?.clientMutationId ===
        data.op.clientMutationId;

      appliedRevisionRef.current = data.revision;

      if (data.socketId === socket.id && matchingInFlightMutation) {
        inFlightMutationRef.current = null;
        if (editorRef.current) {
          lastKnownBodyTextRef.current = obtenerTextoPlanoDelEditor(editorRef.current);
        }
        documentRef.current = {
          ...(documentRef.current ?? {
            id: data.documentId,
            title: DEFAULT_TITLE,
            content: "",
            revision: data.revision,
          }),
          content: data.content,
          revision: data.revision,
        };
        enviarOperacionCuerpoPendiente();
        return;
      }

      const editor = editorRef.current;
      if (!editor) {
        return;
      }

      const shouldRestoreSelection = window.document.activeElement === editor;
      const previousSelection = shouldRestoreSelection
        ? obtenerSeleccionEditor(editor)
        : null;
      const nextSelection = previousSelection
        ? transformarSeleccionPorOperacion(previousSelection, data.op)
        : null;

      aplicarOperacionReemplazoAlEditor(editor, data.op);
      lastKnownBodyTextRef.current = obtenerTextoPlanoDelEditor(editor);

      if (nextSelection) {
        restaurarSeleccionEditor(editor, nextSelection);
        programarEmisionSeleccionCursorDesdeEfecto();
      }

      queuedOpsRef.current = queuedOpsRef.current.map((queuedOp) =>
        transformarOperacionPendientePorOperacion(queuedOp, data.op),
      );
      setRemoteCursors((currentCursors) =>
        transformarCursoresRemotosPorOperacion(currentCursors, data.op),
      );
      documentRef.current = {
        ...(documentRef.current ?? {
          id: data.documentId,
          title: DEFAULT_TITLE,
          content: "",
          revision: data.revision,
        }),
        content: data.content,
        revision: data.revision,
      };
      programarActualizacionSuperposicionesCursoresDesdeEfecto();
    };
    const manejarResincronizacionCuerpo = (data: BodyResyncEvent) => {
      if (data.documentId !== activeDocumentId || !editorRef.current) {
        return;
      }

      queuedOpsRef.current = [];
      queuedDocumentIdRef.current = null;
      inFlightMutationRef.current = null;
      appliedRevisionRef.current = data.revision;
      editorRef.current.textContent = obtenerTextoPlanoDesdeHtml(data.content);
      lastKnownBodyTextRef.current = obtenerTextoPlanoDelEditor(editorRef.current);
      documentRef.current = {
        ...(documentRef.current ?? {
          id: data.documentId,
          title: DEFAULT_TITLE,
          content: "",
          revision: data.revision,
        }),
        content: data.content,
        revision: data.revision,
      };
      programarActualizacionSuperposicionesCursoresDesdeEfecto();
    };
    const manejarActualizacionCursor = (data: {
      documentId: string;
      socketId: string;
      username?: string;
      selection: EditorSelection;
    }) => {
      if (data.documentId !== activeDocumentId || data.socketId === socket.id) {
        return;
      }

      setRemoteCursors((current) => {
        const nextCursor = {
          socketId: data.socketId,
          username: data.username?.trim() || data.socketId.slice(0, 4),
          selection: data.selection,
        };
        const cursorIndex = current.findIndex(
          (cursor) => cursor.socketId === data.socketId,
        );
        if (cursorIndex === -1) {
          return [...current, nextCursor];
        }

        const currentCursor = current[cursorIndex];
        if (
          currentCursor.username === nextCursor.username &&
          sonSeleccionesIguales(currentCursor.selection, nextCursor.selection)
        ) {
          return current;
        }

        return current.map((cursor, index) =>
          index === cursorIndex ? nextCursor : cursor,
        );
      });
    };
    const manejarCursorRemovido = (data: {
      documentId: string;
      socketId: string;
    }) => {
      if (data.documentId !== activeDocumentId) {
        return;
      }

      setRemoteCursors((current) =>
        current.filter((cursor) => cursor.socketId !== data.socketId),
      );
    };

    socket.on("connect", manejarConexion);
    socket.on("disconnect", manejarDesconexion);
    socket.on("document:loaded", manejarDocumentoCargado);
    socket.on("document:edited:title", manejarTituloEditado);
    socket.on("document:edited:body:op", manejarOperacionCuerpoEditada);
    socket.on("document:body:resync", manejarResincronizacionCuerpo);
    socket.on("document:cursor", manejarActualizacionCursor);
    socket.on("document:cursor:removed", manejarCursorRemovido);

    if (socket.connected) {
      manejarConexion();
    }

    return () => {
      limpiarGuardadoCuerpoPendiente();
      socket.off("connect", manejarConexion);
      socket.off("disconnect", manejarDesconexion);
      socket.off("document:loaded", manejarDocumentoCargado);
      socket.off("document:edited:title", manejarTituloEditado);
      socket.off("document:edited:body:op", manejarOperacionCuerpoEditada);
      socket.off("document:body:resync", manejarResincronizacionCuerpo);
      socket.off("document:cursor", manejarActualizacionCursor);
      socket.off("document:cursor:removed", manejarCursorRemovido);
    };
  }, [activeDocumentId]);

  const guardarTitulo = () => {
    const currentDocument = documentRef.current ?? document;
    if (!currentDocument) {
      return;
    }

    const title = titleDraft.trim() ? titleDraft : DEFAULT_TITLE;
    if (title === currentDocument.title) {
      return;
    }

    const nextDocument = { ...currentDocument, title };
    documentRef.current = nextDocument;
    setDocument(nextDocument);
    socket.emit("document:edit:title", { documentId: currentDocument.id, title });
  };

  const guardarUsuario = () => {
    const nextUsername = username.trim() || obtenerNombreUsuarioAleatorio();
    if (nextUsername !== username) {
      setUsername(nextUsername);
      usernameRef.current = nextUsername;
    }

    emitirSeleccionCursor();
  };

  const manejarEntradaCuerpo = () => {
    const currentDocument = documentRef.current;
    const editor = editorRef.current;
    if (!currentDocument || !editor) {
      return;
    }

    const previousText = lastKnownBodyTextRef.current;
    const nextText = obtenerTextoPlanoDelEditor(editor);
    const op = crearOperacionReemplazoDesdeCambioTexto(previousText, nextText);
    lastKnownBodyTextRef.current = nextText;
    if (op) {
      programarGuardadoCuerpo(currentDocument.id, op);
    }
    programarEmisionSeleccionCursor();
  };

  return (
    <main className="app">
      <header className="topbar">
        <div className="topbar-content">
          <input
            className="title"
            value={titleDraft}
            onBlur={guardarTitulo}
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
          <div className="document-info" aria-label="Datos del documento">
            <label className="username-field">
              <span>Usuario</span>
              <input
                value={username}
                onBlur={guardarUsuario}
                onChange={(event) => setUsername(event.target.value)}
                onKeyDown={(event) => {
                  if (event.key === "Enter") {
                    event.preventDefault();
                    event.currentTarget.blur();
                  }
                }}
              />
            </label>
            <span>{socketConnected ? "Conectado" : "Desconectado"}</span>
          </div>
        </div>
      </header>

      <section className="editor" ref={editorWrapperRef}>
        <div
          ref={editorRef}
          className="document-body"
          contentEditable={Boolean(document)}
          onInput={manejarEntradaCuerpo}
          onKeyDown={(event) => {
            if (event.key !== "Enter" || !document) {
              return;
            }

            if (insertarTextoEnSeleccionEditor(event.currentTarget, "\n")) {
              event.preventDefault();
            }
          }}
          onKeyUp={programarEmisionSeleccionCursor}
          onMouseUp={programarEmisionSeleccionCursor}
          role="textbox"
          aria-label="Contenido del documento"
          data-placeholder="Escribe el contenido del documento"
          suppressContentEditableWarning
        />
        <div className="remote-cursors" aria-hidden="true">
          {cursorOverlays.map((cursor) => {
            const style = {
              "--cursor-color": cursor.color,
              left: cursor.left,
              top: cursor.top,
              height: cursor.height,
            } as CSSProperties;

            return (
              <div
                className="remote-cursor"
                key={cursor.socketId}
                style={style}
              >
                <span>{obtenerEtiquetaCursor(cursor)}</span>
              </div>
            );
          },
          )}
        </div>
      </section>
    </main>
  );
}

export default App;
