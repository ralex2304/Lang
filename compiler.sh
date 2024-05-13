#!/bin/bash

HELP_TEXT="\
Usage: compiler.sh [options] file
Options:
    --help            Display this information
    -o <file>         Place the output file into <file>
    -m={spu|x86_64}   Choose architecture (x86-64 is default)
    -l=<file>         Place iolib elf64 object file into <file>
    -S                Generate text assembler listing
"

POSITIONAL_ARGS=()
LISTING=0
OUTPUT_FILE=""
ARCH="x86_64"
IOLIB_FILE="./Programs/doubleiolib.o"

while [[ $# -gt 0 ]]; do
  case $1 in
    -o)
      OUTPUT_FILE="$2"
      shift
      shift
      ;;
    -S)
      LISTING=1
      shift
      ;;
    -m=*)
      ARCH="${i#*=}"
      shift
      ;;
    -l=*)
      IOLIB_FILE="${i#*=}"
      shift
      ;;
    -h|--help)
      echo "${HELP_TEXT}"
      exit 0
      ;;
    -*|--*)
      echo "Unknown option $1"
      exit 1
      ;;
    *)
      POSITIONAL_ARGS+=("$1")
      shift
      ;;
  esac
done

set -- "${POSITIONAL_ARGS[@]}"

INPUT_FILE="$1"
shift

if [[ "${OUTPUT_FILE}" == "" ]]; then
    OUTPUT_FILE="${INPUT_FILE%.*}"
fi

if [[ "${ARCH}" == "x86_64" ]]; then
    LISTING_FILE="${INPUT_FILE%.*}.nasm"
elif [[ "${ARCH}" == "spu" ]]; then
    LISTING_FILE="${INPUT_FILE%.*}.code"
else
    echo "Unsupported arch specified"
    exit 1
fi

AST_FILE="${INPUT_FILE%.*}.tre"
AST_OPT_FILE="${INPUT_FILE%.*}.treopt"
IR_FILE="${INPUT_FILE%.*}.ir"

export LANG=ru_RU.CP1251

luit ./frontend/main -i ${INPUT_FILE} -o ${AST_FILE}

luit ./middleend/main -i ${AST_FILE} -o ${AST_OPT_FILE}

luit ./backend/main -i ${AST_OPT_FILE} -o ${IR_FILE}

if [[ ${LISTING} == 0 ]] then
  luit ./ir_backend/main -i ${IR_FILE} -o ${OUTPUT_FILE} -m ${ARCH}
else
  luit ./ir_backend/main -i ${IR_FILE} -o ${OUTPUT_FILE} -m ${ARCH} -S ${LISTING_FILE}
fi
