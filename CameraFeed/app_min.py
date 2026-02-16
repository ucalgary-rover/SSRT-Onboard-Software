from flask import Flask

app = Flask(__name__)


@app.get("/")
def root():
    return "it lives"


if __name__ == "__main__":
    print("Starting", __file__)
    app.run(host="0.0.0.0", port=5050)
