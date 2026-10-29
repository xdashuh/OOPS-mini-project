"""Local web dashboard for the C++ Library Management System.
The dashboard stores records in the same pipe-delimited library_data.txt file as the C++ app.
"""
from http.server import BaseHTTPRequestHandler, ThreadingHTTPServer
from pathlib import Path
import json

DATA = Path(__file__).with_name("library_data.txt")
HOST, PORT = "127.0.0.1", 8001
PAGE = Path(__file__).with_name("LibraryDashboard.html")


def read_items():
    items = []
    if not DATA.exists():
        return items
    for line in DATA.read_text(encoding="utf-8").splitlines():
        parts = line.split("|")
        if len(parts) != 5 or parts[0] not in {"B", "M"}:
            continue
        try:
            item = {"kind": "Book" if parts[0] == "B" else "Magazine", "id": int(parts[1]), "title": parts[2], "detail": parts[3], "available": parts[4] == "1"}
            if item["kind"] == "Magazine":
                item["detail"] = int(parts[3])
            items.append(item)
        except ValueError:
            continue
    return items


def write_items(items):
    temp = DATA.with_suffix(".tmp")
    temp.write_text("".join(f"{i['kind'][0]}|{i['id']}|{i['title']}|{i['detail']}|{1 if i['available'] else 0}\n" for i in items), encoding="utf-8")
    temp.replace(DATA)


class Handler(BaseHTTPRequestHandler):
    def send_json(self, payload, status=200):
        body = json.dumps(payload).encode()
        self.send_response(status)
        self.send_header("Content-Type", "application/json; charset=utf-8")
        self.send_header("Content-Length", str(len(body)))
        self.end_headers()
        self.wfile.write(body)

    def do_GET(self):
        if self.path == "/":
            body = PAGE.read_bytes()
            self.send_response(200)
            self.send_header("Content-Type", "text/html; charset=utf-8")
            self.send_header("Content-Length", str(len(body)))
            self.end_headers()
            self.wfile.write(body)
        elif self.path == "/api/items":
            self.send_json(read_items())
        else:
            self.send_json({"error": "Not found"}, 404)

    def do_POST(self):
        if self.path != "/api/items":
            self.send_json({"error": "Not found"}, 404)
            return
        try:
            data = json.loads(self.rfile.read(int(self.headers.get("Content-Length", "0"))))
            kind = data.get("kind")
            item_id = int(data.get("id", 0))
            title = str(data.get("title", "")).strip()
            detail = str(data.get("detail", "")).strip()
            if kind not in {"Book", "Magazine"} or item_id <= 0 or not title or not detail:
                raise ValueError("Enter a positive ID, title, and item detail.")
            if "|" in title or "|" in detail or "\n" in title or "\n" in detail:
                raise ValueError("The | character and line breaks are not allowed.")
            if kind == "Magazine":
                detail = str(int(detail))
                if int(detail) <= 0:
                    raise ValueError("Issue number must be positive.")
            items = read_items()
            if any(item["id"] == item_id for item in items):
                raise ValueError("An item with that ID already exists.")
            items.append({"kind": kind, "id": item_id, "title": title, "detail": detail, "available": True})
            write_items(items)
            self.send_json({"created": True}, 201)
        except (ValueError, json.JSONDecodeError) as exc:
            self.send_json({"error": str(exc)}, 400)

    def do_PATCH(self):
        try:
            item_id = int(self.path.split("/")[-1])
            data = json.loads(self.rfile.read(int(self.headers.get("Content-Length", "0"))))
            items = read_items()
            item = next((x for x in items if x["id"] == item_id), None)
            if item is None:
                raise ValueError("Item ID not found.")
            if item["available"] == bool(data.get("available")):
                raise ValueError("Item already has that status.")
            item["available"] = bool(data.get("available"))
            write_items(items)
            self.send_json({"updated": True})
        except (ValueError, json.JSONDecodeError) as exc:
            self.send_json({"error": str(exc)}, 400)

    def log_message(self, fmt, *args):
        print("[library dashboard] " + fmt % args)


if __name__ == "__main__":
    print(f"Library dashboard running at http://{HOST}:{PORT}")
    print("It shares library_data.txt with the C++ app. Press Ctrl+C to stop.")
    ThreadingHTTPServer((HOST, PORT), Handler).serve_forever()
