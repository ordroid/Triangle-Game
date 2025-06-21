import os
import subprocess
import openai

DOC_PATH = "docs/docAid.md"
NO_DIFF = "NO_DIFF"
ERROR = "ERROR"

# Detect diff (for PRs or pushes)
try:
    diff = subprocess.check_output(["git", "diff", "HEAD^", "HEAD"], text=True)
except:
    diff = NO_DIFF

# Check if doc exists
if os.path.exists(DOC_PATH):
    with open(DOC_PATH, "r") as f:
        existing_doc = f.read()

    prompt = f"""This is the current documentation:\n\n{existing_doc}
            \n\nThese are the recent code changes:\n\n{diff}
            \n\nPlease update the documentation accordingly."""
else:
    # Generate new doc from scratch
    code = subprocess.check_output(["git", "ls-files"], text=True)
    file_contents = ""
    for fname in code.strip().splitlines():
        if fname.endswith(('.cpp', '.h', '.py', '.js', '.ts')):  # or whatever file we support
            with open(fname, "r", encoding="utf-8", errors="ignore") as f:
                file_contents += f"\n\n# File: {fname}\n" + f.read()

    prompt = f"""Here is the full codebase:\n\n{file_contents}\n\nGenerate concise but detailed high-level documentation. 
    Focus on key components and structure — not every line or function. Make it suitable for new developers onboarding the project. 
    Include diagrams if helpful."""

# Call OpenAI
openai.api_key = os.getenv("OPENAI_API_KEY")
try:
    response = openai.ChatCompletion.create(
        model="gpt-4.1-nano",
        messages=[
            {"role": "system", "content": "You are an expert at generating concise technical documentation."},
            {"role": "user", "content": prompt},
        ],
    )

    result = response["choices"][0]["message"]["content"]
except Exception as e:
    print("OpenAI API call failed:", e)
    result = ERROR

# Write/update the doc
if result != ERROR:
    os.makedirs("docs", exist_ok=True)
    with open(DOC_PATH, "w") as f:
        f.write(result)
