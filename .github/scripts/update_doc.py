import os
import subprocess
import openai

DOC_PATH = "docs/docAid.md"

# Detect diff (for PRs or pushes)
try:
    diff = subprocess.check_output(["git", "diff", "HEAD^", "HEAD"], text=True)
except:
    diff = ""

# Check if doc exists
if os.path.exists(DOC_PATH):
    with open(DOC_PATH, "r") as f:
        existing_doc = f.read()

    prompt = f"""This is the current documentation:

{existing_doc}

These are the recent code changes:

{diff}

Please update the documentation accordingly.
"""
else:
    # Generate new doc from scratch
    code = subprocess.check_output(["git", "ls-files"], text=True)
    file_contents = ""
    for fname in code.strip().splitlines():
        if fname.endswith(('.cpp', '.h', '.py', '.js', '.ts')):  # or whatever file we support
            with open(fname, "r", encoding="utf-8", errors="ignore") as f:
                file_contents += f"\n\n# File: {fname}\n" + f.read()

    prompt = f"""This is the full codebase:\n\n{file_contents}\n\n\n\nGenerate high-level documentation.
    Keep it short, concise, but detailed enough. Don't give documentation for each line or function,
    only to the neccessary ones. Try to keep it high level, but deep enough for new software developers
    who start to work on this project. Add diagrams if needed."""

# Call OpenAI
openai.api_key = os.getenv("OPENAI_API_KEY")
response = openai.ChatCompletion.create(
    model="gpt-4.1-nano",
    messages=[
        {"role": "system", "content": "You are an expert at generating concise technical documentation."},
        {"role": "user", "content": prompt},
    ],
)

result = response["choices"][0]["message"]["content"]

# Write/update the doc
os.makedirs("docs", exist_ok=True)
with open(DOC_PATH, "w") as f:
    f.write(result)
