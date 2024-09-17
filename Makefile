all: windows macos linux

windows:
  zig cc -Wall -target x86_64-windows meme.c -o meme-windows

macos: macos-arm macos-x86

macos-arm:
  zig cc -Wall -target aarch64-macos meme.c -o meme-apple-arm

macos-x86:
  zig cc -Wall -target x86_64-macos meme.c -o meme-apple-x86

linux: linux-arm linux-x86

linux-arm:
  zig cc -Wall -target aarch64-linux meme.c -o meme-linux-arm

linux-x86:
  zig cc -Wall -target x86_64-linux meme.c -o meme-linux-x86

test:
  zig cc -Wall uses_memory.c -o uses_memory
  ./uses_memory
