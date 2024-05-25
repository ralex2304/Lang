#!/bin/bash
set -o pipefail

onerror() {
  echo "Error raised by ${CUR_PROG}"
  exit 1
}

trap "onerror" ERR

CUR_PROG="compiler.sh"

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

for i in "$@"; do
  case $i in
    -o)
      OUTPUT_FILE="${i+1}"
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
      echo "Unknown option $i"
      exit 1
      ;;
    *)
      POSITIONAL_ARGS+=("$i")
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
    echo "Unsupported arch specified: \"${ARCH}\""
    exit 1
fi

AST_FILE="${INPUT_FILE%.*}.tre"
AST_OPT_FILE="${INPUT_FILE%.*}.treopt"
IR_FILE="${INPUT_FILE%.*}.ir"

CUR_PROG="frontend"
./frontend/main -i ${INPUT_FILE} -o ${AST_FILE} 2>&1 | iconv -f cp1251 -t utf8

CUR_PROG="middleend"
./middleend/main -i ${AST_FILE} -o ${AST_OPT_FILE} 2>&1 | iconv -f cp1251 -t utf8

CUR_PROG="backend"
./backend/main -i ${AST_OPT_FILE} -o ${IR_FILE} 2>&1 | iconv -f cp1251 -t utf8

CUR_PROG="ir_backend"
if [[ ${LISTING} == 0 ]] then
  ./ir_backend/main -i ${IR_FILE} -o ${OUTPUT_FILE} -m ${ARCH} 2>&1 | iconv -f cp1251 -t utf8
else
  ./ir_backend/main -i ${IR_FILE} -o ${OUTPUT_FILE} -m ${ARCH} -S ${LISTING_FILE} 2>&1 | iconv -f cp1251 -t utf8
fi
