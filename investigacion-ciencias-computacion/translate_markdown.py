from __future__ import annotations

import argparse
import hashlib
import json
import os
from pathlib import Path
from typing import Any

from deep_translator import GoogleTranslator


ROOT = Path(__file__).resolve().parent
SOURCE_DIR = ROOT / "docling"
OUTPUT_DIR = ROOT / "translations"
STATE_PATH = OUTPUT_DIR / ".translation_state.json"
MAX_CHARS = 4_500


def file_hash(path: Path) -> str:
    return hashlib.sha256(path.read_bytes()).hexdigest()


def load_state() -> dict[str, Any]:
    if not STATE_PATH.exists():
        return {"files": {}}
    return json.loads(STATE_PATH.read_text(encoding="utf-8"))


def save_state(state: dict[str, Any]) -> None:
    OUTPUT_DIR.mkdir(parents=True, exist_ok=True)
    temp_path = STATE_PATH.with_suffix(".tmp")
    temp_path.write_text(json.dumps(state, indent=2, ensure_ascii=False), encoding="utf-8")
    os.replace(temp_path, STATE_PATH)


def split_markdown(markdown: str) -> list[dict[str, str]]:
    chunks: list[dict[str, str]] = []
    current: list[str] = []
    current_len = 0
    in_fence = False
    fence: list[str] = []

    def flush_current() -> None:
        nonlocal current, current_len
        if current:
            chunks.append({"kind": "text", "content": "".join(current)})
            current = []
            current_len = 0

    for line in markdown.splitlines(keepends=True):
        if line.lstrip().startswith("```"):
            if in_fence:
                fence.append(line)
                chunks.append({"kind": "raw", "content": "".join(fence)})
                fence = []
                in_fence = False
            else:
                flush_current()
                fence = [line]
                in_fence = True
            continue

        if in_fence:
            fence.append(line)
            continue

        if current_len + len(line) > MAX_CHARS and current:
            flush_current()

        current.append(line)
        current_len += len(line)

        if line.strip() == "" and current_len >= MAX_CHARS // 2:
            flush_current()

    if fence:
        chunks.append({"kind": "raw", "content": "".join(fence)})
    flush_current()
    return chunks


def translate_chunk(translator: GoogleTranslator, chunk: dict[str, str]) -> str:
    if chunk["kind"] == "raw" or not chunk["content"].strip():
        return chunk["content"]
    return translator.translate(chunk["content"])


def output_path_for(source_path: Path) -> Path:
    return OUTPUT_DIR / source_path.relative_to(SOURCE_DIR)


def translate_file(source_path: Path, translator: GoogleTranslator, state: dict[str, Any]) -> str:
    relative_key = source_path.relative_to(SOURCE_DIR).as_posix()
    output_path = output_path_for(source_path)
    part_path = output_path.with_suffix(output_path.suffix + ".part")
    source_hash = file_hash(source_path)
    chunks = split_markdown(source_path.read_text(encoding="utf-8"))

    file_state = state["files"].get(relative_key, {})
    if output_path.exists() and file_state.get("done") and file_state.get("hash") == source_hash:
        return "skipped"

    if output_path.exists() and not part_path.exists() and not file_state:
        state["files"][relative_key] = {
            "hash": source_hash,
            "next_chunk": len(chunks),
            "total_chunks": len(chunks),
            "done": True,
        }
        save_state(state)
        return "skipped"

    can_resume = (
        part_path.exists()
        and file_state.get("hash") == source_hash
        and file_state.get("total_chunks") == len(chunks)
        and not file_state.get("done")
    )
    next_chunk = int(file_state.get("next_chunk", 0)) if can_resume else 0

    output_path.parent.mkdir(parents=True, exist_ok=True)
    if not can_resume:
        part_path.write_text("", encoding="utf-8")
        state["files"][relative_key] = {
            "hash": source_hash,
            "next_chunk": 0,
            "total_chunks": len(chunks),
            "done": False,
        }
        save_state(state)

    with part_path.open("a", encoding="utf-8") as output_file:
        for index in range(next_chunk, len(chunks)):
            output_file.write(translate_chunk(translator, chunks[index]))
            output_file.flush()
            os.fsync(output_file.fileno())

            state["files"][relative_key]["next_chunk"] = index + 1
            save_state(state)

    os.replace(part_path, output_path)
    state["files"][relative_key]["done"] = True
    save_state(state)
    return "translated" if next_chunk == 0 else "resumed"


def main() -> None:
    global SOURCE_DIR, OUTPUT_DIR, STATE_PATH

    parser = argparse.ArgumentParser(description="Translate docling Markdown files into Spanish.")
    parser.add_argument("--source", type=Path, default=SOURCE_DIR, help="Directory with Markdown files.")
    parser.add_argument("--output", type=Path, default=OUTPUT_DIR, help="Directory for translated files and progress state.")
    args = parser.parse_args()

    SOURCE_DIR = args.source.resolve()
    OUTPUT_DIR = args.output.resolve()
    STATE_PATH = OUTPUT_DIR / ".translation_state.json"

    state = load_state()
    state.setdefault("files", {})
    translator = GoogleTranslator(source="auto", target="es")

    for source_path in sorted(SOURCE_DIR.rglob("*.md")):
        result = translate_file(source_path, translator, state)
        print(f"{result}: {source_path.relative_to(SOURCE_DIR)}")


if __name__ == "__main__":
    main()
