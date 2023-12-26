Import("env")

# Install the required module
env.Execute("pip install pyyaml")
env.Execute("pip install jinja2")

env.Execute(f"python scripts/auto_gen_memo.py")

