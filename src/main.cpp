extern "C" {

// Minimal stub for _swprintf so that newlib's strftime can link.
// This stub does NOTHING useful but satisfies the linker.
// If strftime tries to format wide strings, it will simply write nothing and return -1.
int _swprintf(wchar_t *ws, const wchar_t *fmt, ...) {
    (void)ws;
    (void)fmt;
    return -1;
}

} // extern "C"




// for setenv from <stdlib.h>
#define _POSIX_C_SOURCE 200112L
#include "addresses.h"
#include "gui.hpp"
#include "impl.hpp"
#include "loaders/binary/loader.hpp"
#include "loaders/elf/loader.hpp"
#include "loaders/interface.hpp"
#include "strconv.hpp"
#include <alloca.h>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <memory>
#include <sdk/calc/calc.h>
#include <sdk/os/debug.h>
#include <sdk/os/gui.h>
#include <sdk/os/lcd.h>
#include <stdexcept>
#include <sdk/os/input.h>
#include <fstream>




void do_override() {
  const auto guard_len = std::strlen(safe_guard);

  constexpr char prefix[] = "\\fls0\\addresses.";
  constexpr char suffix[] = ".override"; // incl \0

  const auto path = reinterpret_cast<char *>(
      alloca(sizeof(prefix) - 1 + guard_len + sizeof(suffix)));
  std::memcpy(path, prefix, sizeof(prefix) - 1);
  std::memcpy(path + sizeof(prefix) - 1, safe_guard, guard_len);
  std::memcpy(path + sizeof(prefix) - 1 + guard_len, suffix, sizeof(suffix));

  const auto fd = std::fopen(path, "rb");
  if (!fd)
    return;

  std::fseek(fd, 0, SEEK_END);
  const auto file_size = std::ftell(fd);
  std::fseek(fd, 0, SEEK_SET);

  const auto guard = reinterpret_cast<char *>(alloca(guard_len));
  if (std::fread(guard, sizeof(char) * (guard_len), 1, fd) != 1)
    throw std::runtime_error("Failed to read guard");
  if (!check_safe_guard(guard))
    throw std::runtime_error("Invalid guard");

  const unsigned long size = file_size - (guard_len) + (addresses_size);
  const auto buf = new uint8_t[size];
  if (!buf)
    throw std::runtime_error("Failed to allocate override");
  std::memcpy(buf, addresses, addresses_size);

  const auto bytes_to_read = file_size - guard_len;
  if (std::fread(buf + addresses_size, bytes_to_read, 1, fd) != 1)
    throw std::runtime_error("Failed to read override");

  if (std::fclose(fd) != 0)
    throw std::runtime_error("Failed to close override");

  if (!relink_sdk(buf, size))
    throw std::runtime_error("Failed to relink sdk");

  if (!relink_libc(buf, size))
    throw std::runtime_error("Failed to relink libc");

  {
    char cbuf[sizeof(std::uintptr_t) * 2 + 1];
    ultohexstr(reinterpret_cast<std::uintptr_t>(buf), cbuf);
    cbuf[sizeof(cbuf) - 1] = '\0';
    setenv("HHK_SYMBOL_TABLE", cbuf, 1);
  }
  {
    char cbuf[sizeof(std::size_t) * 2 + 1];
    ultohexstr(reinterpret_cast<std::size_t>(buf), cbuf);
    cbuf[sizeof(cbuf) - 1] = '\0';
    setenv("HHK_SYMBOL_TABLE_LEN", cbuf, 1);
  }

  return;
}

void init_pwd() {
  std::ofstream pwdi("\\fls0\\pwd.txt");
  if (! pwdi) {
    throw std::runtime_error("Failed to create pwd.txt (Password File)");
  }
  pwdi << 4 << std::endl;
  pwdi << 3 << std::endl;
  pwdi << 1 << std::endl;
  pwdi << 9 << std::endl;
  pwdi.close();
}

bool read_pwd(int &v1, int &v2, int &v3, int &v4) {
  std::ifstream pwd("\\fls0\\pwd.txt");
  if (!pwd) {
    // attempt to create default file, then reopen
    init_pwd();
    pwd.open("\\fls0\\pwd.txt");
    if (!pwd) {
      return false;
    }
  }

  // Use chained extraction; this returns the stream so we can test it
  if (pwd >> v1 >> v2 >> v3 >> v4) {
    pwd.close();
    return true;
  } else {
    pwd.close();
    return false;
  }
}
int main() {
  int val_1 = 0, val_2 = 0, val_3 = 0, val_4 = 0;
  if (!read_pwd(val_1, val_2, val_3, val_4)) {
    // handle failure; either proceed or abort
    // Here we abort because password couldn't be read/created
    std::fprintf(stderr, "Failed to read/create pwd.txt\n");
    return 1;
  }

  // password sequence
  const int pwd_seq[4] = { val_1, val_2, val_3, val_4 };
  int seq_index = 0;

  // Keep polling for input until either password entered or other flow continues
  // Note: GetInput signature is: int GetInput(struct Input_Event* event, uint32_t unknown1, uint32_t unknown2)
  // The header indicates "0xFFFFFFFF or 0" for wait param; and 0x10 for unknown2.
  Input_Event event;
  // Zero the event as recommended
  std::memset(&event, 0, sizeof(event));

  // Simple loop - adjust as needed; **do not block forever** in real app
  for (;;) {
    // wait for an event (blocking). If you want non-blocking, pass different args per API doc.
    if (GetInput(&event, 0xFFFFFFFF, 0x10) != 0) {
      // GetInput is documented to return 0; if nonzero treat as continue
      continue;
    }

    // Only handle key events
    if (event.type != Input_EventType::EVENT_KEY) {
      // handle non-key events or break out to main application flow
      // For now, continue looping
      continue;
    }

    // We only care about KEY_PRESSED for a password sequence; adjust as required
    if (event.data.key.direction != KEY_PRESSED)
      continue;

    int pressed = static_cast<int>(event.data.key.keyCode);

    // Check pressed against the current expected element of the sequence
    if (pressed == pwd_seq[seq_index]) {
      ++seq_index;
      if (seq_index == 4) {
        // full sequence matched
        do_override();
        seq_index = 0; // reset if you want future triggers
        break; // or continue depending on whether you want to keep running
      }
    } else {
      // mismatch: reset sequence (or maybe check for prefix matches)
      seq_index = 0;
      // Optionally: if this key equals first element, set seq_index=1
      if (pressed == pwd_seq[0]) seq_index = 1;
    }

    // After password handling we can break to the rest of the program
    // Or continue; I break to continue starting the GUI in your original flow
    if (/* some condition to move on to GUI */ false) {
      break;
    }
  }

  // The remainder of your original main - discovery, GUI, etc.

  std::unique_ptr<Executable> choosen;
  {
    std::forward_list<std::unique_ptr<Executable>> list;
    if (std::strcmp(reinterpret_cast<const char *>(addresses),
                    "2017.0512.1515") == 0) // 2000 aka original hhk2
      discover<BinaryLoader>::run(list);
    discover<ELFLoader>::run(list);
    choosen = do_gui(list);
  }

  if (!choosen)
    return 0;

  calcExit();
  choosen->load();
  auto ret = choosen->execute();
  choosen->unload();
  calcInit();

  return ret;
}
