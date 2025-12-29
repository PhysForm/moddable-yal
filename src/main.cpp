




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

static int pwd(int placement) {
  int fd = File_Open("\\\\fls0\\passwd.txt", FILE_OPEN_READ);
  if(fd < 0) {
    int fdcreate = File_Open("\\\\fls0\\passwd.txt", FILE_OPEN_WRITE | FILE_OPEN_CREATE);
    if (fdcreate < 0) {
      return 0;
    }
    uint8_t pwd[] = {0,0,0,0};
    int ret1 = File_Write(fdcreate, pwd, sizeof(pwd));
    if (ret1 < 0) {
      File_Close(fdcreate);
      return 0;
    }
    File_Close(fdcreate);
    ret1 = File_Close(fdcreate);
    if (ret1 < 0) {
      return 0;
    }
  }
  uint8_t read[4];
  int ret = File_Read(fd, read, sizeof(read));
  if (ret < 0) {
    File_Close(fd);
    return  0;
  }
  int key1 = 0;
  switch(read[0]) {
    case 0:
      key1 = KEYCODE_0;
      break;
    case 1:
      key1 = KEYCODE_1;
      break;
    case 2: 
      key1 = KEYCODE_2;
      break;
    case 3:
      key1 = KEYCODE_3;
      break;
    case 4:
      key1 = KEYCODE_4;
      break;
    case 5:
      key1 = KEYCODE_5;
      break;
    case 6:
      key1 = KEYCODE_6;
      break;
    case 7:
      key1 = KEYCODE_7;
      break;
    case 8:
      key1 = KEYCODE_8;
      break;
    case 9:
      key1 = KEYCODE_9;
      break;
    default:
      return 0;
  }; int key2 = 0;
  switch(read[1]) {
    case 0:
      key2 = KEYCODE_0;
      break;
    case 1:
      key2 = KEYCODE_1;
      break;
    case 2: 
      key2 = KEYCODE_2;
      break;
    case 3:
      key2 = KEYCODE_3;
      break;
    case 4:
      key2 = KEYCODE_4;
      break;
    case 5:
      key2 = KEYCODE_5;
      break;
    case 6:
      key2 = KEYCODE_6;
      break;
    case 7:
      key2 = KEYCODE_7;
      break;
    case 8:
      key2 = KEYCODE_8;
      break;
    case 9:
      key2 = KEYCODE_9;
      break;
    default:
      return 0;
  }; int key3 = 0;
  switch(read[2]) {
    case 0:
      key3 = KEYCODE_0;
      break;
    case 1:
      key3 = KEYCODE_1;
      break;
    case 2: 
      key3 = KEYCODE_2;
      break;
    case 3:
      key3 = KEYCODE_3;
      break;
    case 4:
      key3 = KEYCODE_4;
      break;
    case 5:
      key3 = KEYCODE_5;
      break;
    case 6:
      key3 = KEYCODE_6;
      break;
    case 7:
      key3 = KEYCODE_7;
      break;
    case 8:
      key3 = KEYCODE_8;
      break;
    case 9:
      key3 = KEYCODE_9;
      break;
    default:
      return 0;
  }; int key4 = 0;
  switch(read[3]) {
    case 0:
      key4 = KEYCODE_0;
      break;
    case 1:
      key4 = KEYCODE_1;
      break;
    case 2: 
      key4 = KEYCODE_2;
      break;
    case 3:
      key4 = KEYCODE_3;
      break;
    case 4:
      key4 = KEYCODE_4;
      break;
    case 5:
      key4 = KEYCODE_5;
      break;
    case 6:
      key4 = KEYCODE_6;
      break;
    case 7:
      key4 = KEYCODE_7;
      break;
    case 8:
      key4 = KEYCODE_8;
      break;
    case 9:
      key4 = KEYCODE_9;
      break;
    default:
      return 0;
  }

  switch(placement) {
    case 1:
      return key1;
      break;
    case 2:
      return key2;
      break;
    case 3:
      return key3;
      break;
    case 4:
      return key4;
      break;
    default:
      return 0;
  }

}


int main() {
  bool firstKeyPressed = false;
  bool secondKeyPressed = false;
  bool thirdKeyPressed = false;

  {
      struct Input_Event event;
      while (true) {
          GetInput(&event, 0xFFFFFFFF, 0x10);

          if (event.type == EVENT_KEY && event.data.key.direction == KEY_PRESSED) {

              if (!firstKeyPressed) {
                  if (event.data.key.keyCode == pwd(1)) {
                      firstKeyPressed = true;
                  } else {
                      firstKeyPressed = false;
                      secondKeyPressed = false;
                      thirdKeyPressed = false;
                  }
              }

              else if (!secondKeyPressed) {
                  if (event.data.key.keyCode == pwd(2)) {
                      secondKeyPressed = true;
                  } else {
                      firstKeyPressed = false;
                      secondKeyPressed = false;
                      thirdKeyPressed = false;
                  }
              }

              else if (!thirdKeyPressed) {
                  if (event.data.key.keyCode == pwd(3)) {
                      thirdKeyPressed = true;
                  } else {
                      firstKeyPressed = false;
                      secondKeyPressed = false;
                      thirdKeyPressed = false;
                  }
              }

              else {
                  if (event.data.key.keyCode == pwd(4)) {
                      break;
                  } else {
                      firstKeyPressed = false;
                      secondKeyPressed = false;
                      thirdKeyPressed = false;
                  }
              }
          }
      }
  }
 
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
