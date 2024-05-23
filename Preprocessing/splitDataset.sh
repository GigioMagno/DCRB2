#!/bin/bash

PART_SIZE="10m"
INPUT_DIR="/Users/svitol/Desktop/DCRBproject/binary_tree/Preprocessing/Dataset/"
OUTPUT_DIR="/Users/svitol/Desktop/DCRBproject/binary_tree/Preprocessing/Dataset/split/"

mkdir -p "$OUTPUT_DIR"

for FILE in "$INPUT_DIR"/*.txt; do
  FILENAME=$(basename "$FILE" .txt)
  split -b "$PART_SIZE" "$FILE" "$OUTPUT_DIR/${FILENAME}_part_"

  # Rinomina i file suddivisi con estensione .txt e formattazione nome_input1.txt, nome_input2.txt, ecc.
  COUNTER=1
  for PART in "$OUTPUT_DIR/${FILENAME}_part_"*; do
    mv "$PART" "$OUTPUT_DIR/${FILENAME}_input${COUNTER}.txt"
    COUNTER=$((COUNTER + 1))
  done
done

echo "Divisione completata!"
