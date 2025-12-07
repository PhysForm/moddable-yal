




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
#include <sdk/os/file.h>




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




int main() {
  int pwdd = File_Open("\\\\fls0\\passwd\\passwd.txt",FILE_OPEN_READ);
  if (pwdd < 0) {
    pwdd = File_Open("\\\\fls0\\passwd.txt",FILE_OPEN_WRITE | FILE_OPEN_CREATE);
    if (pwdd < 0) {
      throw std::runtime_error("Failed to Read Password!");
    }

    uint8_t buf[7] = {1,0x0A,2,0x0A,3,0x0A,4};
    int rest = File_Write(pwdd,buf,sizeof(buf));
    if (rest < 0) {
      File_Close(pwdd);
      throw std::runtime_error("Failed to Read Password!");
    }
  }
  uint8_t buf[7];
  int pwd = File_Read(pwdd, buf, sizeof(buf));
  if (pwd < 0) {
    File_Close(pwdd);
    const uint8_t default_buf[7] = {1,0x0A,2,0x0A,3,0x0A,4};
    std::memcpy(buf, default_buf, sizeof(buf));
    goto continued;
  }
  
  continued:

  int retl = File_Close(pwdd);
  if (retl < 0) {
    throw std::runtime_error("Failed to Read Password!");
  }
  const uint8_t pwd_buf[4] = {buf[0],buf[2],buf[4],buf[6]};
  std::memcpy(buf,pwd_buf,sizeof(pwd_buf));

  // Keep polling for input until either password entered or other flow continues
  // Note: GetInput signature is: int GetInput(struct Input_Event* event, uint32_t unknown1, uint32_t unknown2)
  // The header indicates "0xFFFFFFFF or 0" for wait param; and 0x10 for unknown2.
  Input_Event event;
  // Zero the event as recommended
  std::memset(&event, 0, sizeof(event));

  GetInput(&event, 0xFFFFFFFF, 0x10);
if (event.type == EVENT_KEY) {
    if (event.data.key.direction) {
      if (event.data.key.keyCode == KEYCODE_1) {
        if (buf[0] != 1) {
          // Incorrect password
          while (true) {
            // Infinite loop to simulate lockout
            GetInput(&event, 0xFFFFFFFF, 0x10);
          }
        }
      } else if (event.data.key.keyCode == KEYCODE_2) {
        if (buf[1] != 2) {
          // Incorrect password
          while (true) {
            // Infinite loop to simulate lockout
            GetInput(&event, 0xFFFFFFFF, 0x10);
          }
        }
      } else if (event.data.key.keyCode == KEYCODE_3) {
        if (buf[2] != 3) {
          // Incorrect password
          while (true) {
            // Infinite loop to simulate lockout
            GetInput(&event, 0xFFFFFFFF, 0x10);
          }
        }
      } else if (event.data.key.keyCode == KEYCODE_4) {
        if (buf[3] != 4) {
          // Incorrect password
          while (true) {
            // Infinite loop to simulate lockout
            GetInput(&event, 0xFFFFFFFF, 0x10);
          }
        }
      }
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
