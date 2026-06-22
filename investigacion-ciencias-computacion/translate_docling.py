from __future__ import annotations

import json
from dataclasses import dataclass
from pathlib import Path
from typing import Iterable


DOCS_DIR = Path("docling")
TRANSLATIONS_DIR = Path("translations")
STATE_FILE = TRANSLATIONS_DIR / ".translation_state.json"
TEMP_SUFFIX = ".part"
MAX_CHARS = 3500


@dataclass
class Piece:
    kind: str
    text: str


def get_translator():
    try:
        from deep_translator import GoogleTranslator
    except ImportError as error:  # pragma: no cover - user environment guidance
        raise SystemExit(
            "Missing dependency: deep-translator. Install it with: pip install deep-translator"
        ) from error

    return GoogleTranslator(source="auto", target="es")


def load_state() -> dict:
    if not STATE_FILE.exists():
        return {"files": {}}

    return json.loads(STATE_FILE.read_text(encoding="utf-8"))


def save_state(state: dict) -> None:
    TRANSLATIONS_DIR.mkdir(parents=True, exist_ok=True)
    STATE_FILE.write_text(
        json.dumps(state, ensure_ascii=False, indent=2),
        encoding="utf-8",
    )


def split_text(text: str, max_chars: int = MAX_CHARS) -> list[str]:
    if len(text) <= max_chars:
        return [text]

    chunks: list[str] = []
    current = ""

    for line in text.splitlines(keepends=True):
        if len(line) > max_chars:
            if current:
                chunks.append(current)
                current = ""

            start = 0
            while start < len(line):
                chunks.append(line[start : start + max_chars])
                start += max_chars
            continue

        if len(current) + len(line) > max_chars:
            chunks.append(current)
            current = line
            continue

        current += line

    if current:
        chunks.append(current)

    return chunks


def build_pieces(content: str) -> list[Piece]:
    pieces: list[Piece] = []
    buffer: list[str] = []
    in_code_block = False

    def flush_buffer() -> None:
        if not buffer:
            return

        text = "".join(buffer)
        for chunk in split_text(text):
            pieces.append(Piece(kind="translate", text=chunk))
        buffer.clear()

    for line in content.splitlines(keepends=True):
        stripped = line.lstrip()
        if stripped.startswith("```") or stripped.startswith("~~~"):
            flush_buffer()
            pieces.append(Piece(kind="literal", text=line))
            in_code_block = not in_code_block
            continue

        if in_code_block:
            pieces.append(Piece(kind="literal", text=line))
            continue

        if stripped.startswith("|") or stripped.startswith(">"):
            flush_buffer()
            pieces.append(Piece(kind="translate", text=line))
            continue

        buffer.append(line)

    flush_buffer()
    return pieces


def iter_markdown_files() -> Iterable[Path]:
    return sorted(path for path in DOCS_DIR.rglob("*.md") if path.is_file())


def append_text(path: Path, text: str) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    with path.open("a", encoding="utf-8") as file:
        file.write(text)


def translate_piece(translator, piece: Piece) -> str:
    if piece.kind == "literal":
        return piece.text

    if not piece.text.strip():
        return piece.text

    translated = translator.translate(piece.text)
    return translated if translated is not None else piece.text


def translate_file(source_path: Path, translator, state: dict) -> None:
    relative_path = source_path.relative_to(DOCS_DIR)
    output_path = TRANSLATIONS_DIR / relative_path
    temp_path = output_path.with_suffix(output_path.suffix + TEMP_SUFFIX)

    content = source_path.read_text(encoding="utf-8")
    pieces = build_pieces(content)

    file_state = state.setdefault("files", {}).setdefault(
        str(relative_path),
        {
            "status": "pending",
            "next_piece": 0,
            "total_pieces": len(pieces),
            "temp_output": str(temp_path),
            "final_output": str(output_path),
        },
    )

    if file_state.get("status") == "done" and output_path.exists():
        print(f"[skip] {relative_path}")
        return

    if file_state.get("total_pieces") != len(pieces):
        file_state["next_piece"] = 0
        file_state["total_pieces"] = len(pieces)
        if temp_path.exists():
            temp_path.unlink()

    file_state["status"] = "in_progress"
    save_state(state)

    start_index = file_state.get("next_piece", 0)
    if start_index == 0 and temp_path.exists():
        temp_path.unlink()

    for index in range(start_index, len(pieces)):
        translated = translate_piece(translator, pieces[index])
        append_text(temp_path, translated)
        file_state["next_piece"] = index + 1
        save_state(state)
        print(f"[progress] {relative_path} ({index + 1}/{len(pieces)})")

    output_path.parent.mkdir(parents=True, exist_ok=True)
    temp_path.replace(output_path)
    file_state["status"] = "done"
    save_state(state)
    print(f"[done] {relative_path}")


def main() -> None:
    if not DOCS_DIR.exists():
        raise SystemExit(f"Directory not found: {DOCS_DIR}")

    TRANSLATIONS_DIR.mkdir(parents=True, exist_ok=True)
    state = load_state()
    translator = get_translator()

    files = list(iter_markdown_files())
    if not files:
        print("No markdown files found.")
        return

    for path in files:
        translate_file(path, translator, state)

    print("All translations completed.")


if __name__ == "__main__":
    main()
