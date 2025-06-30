FROM ubuntu@sha256:79efa276fdefa2ee3911db29b0608f8c0561c347ec3f4d4139980d43b168d991

# Update the package database and install required tools
# FIX 2: Added 'curl' for downloading rustup script.
RUN apt-get update && apt-get install -y --no-install-recommends \
    build-essential gcc-15 g++-15 curl \
    python3 python3-pip pipx git less vim sudo cmake \
    clang-format clangd-19 nodejs npm \
    && rm -rf /var/lib/apt/lists/*

# Update gcc/g++ version to the latest ones
RUN ln -fs /usr/bin/g++-15 /usr/bin/g++
RUN ln -fs /usr/bin/gcc-15 /usr/bin/gcc

# Create a non-root user 'dev'
RUN useradd -ms /bin/bash dev \
    && echo "dev ALL=(ALL) NOPASSWD: ALL" >> /etc/sudoers.d/dev \
    && chmod 0440 /etc/sudoers.d/dev

# --- Build and install tree-sitter from source ---
WORKDIR /tmp
RUN git clone https://github.com/tree-sitter/tree-sitter
WORKDIR /tmp/tree-sitter
RUN make -j4
RUN sudo make install

# FIX 2: Install Rust/Cargo via rustup, not apt.
# Install Rust and Cargo using the official rustup script
RUN curl --proto '=https' --tlsv1.2 -sSf https://sh.rustup.rs | sh -s -- -y
# Add cargo to the PATH for subsequent RUN commands
ENV PATH="/root/.cargo/bin:${PATH}"
# Install tree-sitter CLI using the now-available cargo command
RUN cargo install tree-sitter-cli --root /usr/local

# --- Build and install tree-sitter-python parser ---
WORKDIR /tmp
RUN git clone https://github.com/tree-sitter/tree-sitter-python
WORKDIR /tmp/tree-sitter-python/
RUN tree-sitter generate
RUN gcc -shared -o libtree-sitter-python.so -Isrc src/parser.c -fPIC
RUN sudo mkdir -p /usr/local/lib/tree-sitter/
RUN sudo mv libtree-sitter-python.so /usr/local/lib/tree-sitter/python.so

# --- Configure tree-sitter for root ---
WORKDIR /
RUN rm -rf /tmp/*
RUN mkdir -p /root/.config/tree-sitter
RUN echo '{"parser-directories":["/usr/local/lib/tree-sitter"]}' > /root/.config/tree-sitter/config.json
RUN chmod -R 777 /root/.config

# Switch to the 'dev' user
USER dev
WORKDIR /home/dev

# --- User specific configurations ---
RUN git config --global core.editor "code --wait"

# Install conan through pipx
RUN pipx install conan
RUN pipx ensurepath

# Copy conan profile
COPY default /home/dev/.conan2/profiles/default
RUN sudo chown -R dev:dev /home/dev/.conan2
RUN sudo chmod 666 /home/dev/.conan2/profiles/default

# Set the default command
CMD ["/bin/bash"]