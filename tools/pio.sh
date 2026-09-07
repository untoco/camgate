#!/bin/sh
set -eu

project_dir=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)
export PIO_HOME_DIR="$project_dir/.tooling/platformio-home"

exec "$project_dir/.tooling/platformio/bin/pio" "$@"
