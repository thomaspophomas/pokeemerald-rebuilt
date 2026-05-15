#!/usr/bin/env sh
set -eu

require_file() {
    if [ ! -f "$1" ]; then
        echo "Missing required file: $1" >&2
        exit 1
    fi
}

require_heading() {
    file="$1"
    heading="$2"
    if ! grep -q "^## $heading$" "$file"; then
        echo "Missing heading '$heading' in $file" >&2
        exit 1
    fi
}

require_file README.md
require_file AGENTS.md
require_file CLAUDE.md

for file in README.md AGENTS.md CLAUDE.md; do
    if ! grep -q "<!-- last_updated:" "$file"; then
        echo "Missing last_updated marker in $file" >&2
        exit 1
    fi
    require_heading "$file" "TL;DR"
    require_heading "$file" "Architecture"
    require_heading "$file" "Discovery Order"
done

require_heading AGENTS.md "Conventions"
require_heading AGENTS.md "Do NOT"
require_heading CLAUDE.md "Conventions"
require_heading CLAUDE.md "Do NOT"

echo "Documentation structure OK"
