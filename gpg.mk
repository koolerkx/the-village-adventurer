# Makefile for GPG Encryption and Decryption
# -------------------------------------------

# --- Configuration ---
GPG_EMAILS_FILE := .gpg-emails
GPG_FILES_FILE  := .gpg-files

# --- GPG Command Options ---
GPG_OPTS := --trust-model always

# --- Helper Variables (Do not edit) ---
GPG_RECIPIENTS := $(shell cat $(GPG_EMAILS_FILE) 2>/dev/null | xargs -I {} echo -r {})
PLAINTEXT_FILES := $(shell cat $(GPG_FILES_FILE) 2>/dev/null)
ENCRYPTED_FILES := $(patsubst %,%.gpg,$(PLAINTEXT_FILES))

# --- Targets ---

# Using .PHONY tells 'make' that these are command recipes, not files to be built.
# This is crucial for avoiding circular dependency errors with file transformations.
.PHONY: all encrypt-all decrypt-all clean help

# Default target
all: help

# This target's recipe is a simple shell loop. 'make' executes the loop
# without trying to resolve individual file dependencies, which prevents the error.
encrypt-all: $(GPG_FILES_FILE) $(GPG_EMAILS_FILE)
	@echo "--- Encrypting files for recipients... ---"
	@if [ -z "$(PLAINTEXT_FILES)" ]; then \
		echo "Warning: No files listed in $(GPG_FILES_FILE). Nothing to do."; \
		exit 0; \
	fi
	@for file in $(PLAINTEXT_FILES); do \
		if [ -f "$$file" ]; then \
			echo "Encrypting $$file -> $$file.gpg"; \
			gpg --encrypt $(GPG_OPTS) $(GPG_RECIPIENTS) -o "$$file.gpg" "$$file"; \
		else \
			echo "Warning: Source file '$$file' not found. Skipping."; \
		fi; \
	done
	@echo "--- Encryption complete. ---"

# This target also uses a shell loop for the same reason.
decrypt-all:
	@echo "--- Attempting to decrypt files... ---"
	@if [ -z "$(PLAINTEXT_FILES)" ]; then \
		echo "Warning: No files listed in $(GPG_FILES_FILE). Nothing to do."; \
		exit 0; \
	fi
	@for file in $(PLAINTEXT_FILES); do \
		if [ -f "$$file.gpg" ]; then \
			echo "Decrypting $$file.gpg -> $$file"; \
			gpg --yes --decrypt $(GPG_OPTS) -o "$$file" "$$file.gpg"; \
		else \
			echo "Skipping: Encrypted file $$file.gpg not found."; \
		fi; \
	done
	@echo "--- Decryption complete. ---"

clean:
	@echo "--- Cleaning up encrypted files (*.gpg)... ---"
	@# This check prevents an error if no files were ever encrypted
	@if [ ! -z "$(ENCRYPTED_FILES)" ]; then \
		rm -f $(ENCRYPTED_FILES); \
	fi
	@echo "--- Cleanup complete. ---"

help:
	@echo "Available commands:"
	@echo "  make encrypt-all : Encrypts files listed in '$(GPG_FILES_FILE)'."
	@echo "  make decrypt-all : Decrypts files corresponding to the list in '$(GPG_FILES_FILE)'."
	@echo "  make clean       : Removes all generated *.gpg files."
	@echo "  make help        : Shows this help message."