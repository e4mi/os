#pragma once
#include "os.c" /* OsEmit(), OsCopy(), OsPrint(), OsStringCompare() */

#define SECTOR_SIZE 512

typedef struct {
  char filename[64];
  unsigned short first_sector, next_entry_sector;
  unsigned int size;
} FileEntry;

static unsigned char Table[SECTOR_SIZE];

void FileList();
int FileRead(char *filename, unsigned char *buffer);
int FileWrite(char *filename, unsigned char *buffer, unsigned int size);
static unsigned short FreeDirSector(), FreeSector();
static void ReadSector(unsigned int lba, unsigned char *buffer);
static void WriteSector(unsigned int lba, unsigned char *buffer);
static void LoadTable() { ReadSector(2, Table); }

void FileList() {
  unsigned char sector[SECTOR_SIZE];
  for (unsigned short dir_sector = 1; dir_sector != 0xFFFF;) {
    ReadSector(dir_sector, sector);
    FileEntry *entry = (FileEntry *)sector;
    for (int i = 0; i < SECTOR_SIZE / sizeof(FileEntry) && entry[i].filename[0];
         i++)
      OsPrint(entry[i].filename), OsEmit('\n');
    dir_sector = entry->next_entry_sector;
  }
}

int FileRead(char *filename, unsigned char *buffer) {
  unsigned char sector[SECTOR_SIZE];
  for (unsigned short dir_sector = 1; dir_sector != 0xFFFF;) {
    ReadSector(dir_sector, sector);
    FileEntry *entry = (FileEntry *)sector;
    for (int i = 0; i < SECTOR_SIZE / sizeof(FileEntry); i++)
      if (OsStringCompare(entry[i].filename, filename) == 0) {
        for (unsigned short s = entry[i].first_sector; s != 0xFFFF;
             s = Table[s])
          ReadSector(s, buffer), buffer += SECTOR_SIZE;
        return entry[i].size;
      }
    dir_sector = entry->next_entry_sector;
  }
  return -1;
}

int FileWrite(char *filename, unsigned char *buffer, unsigned int size) {
  unsigned char sector[SECTOR_SIZE];
  FileEntry *entry = (FileEntry *)sector;
  ReadSector(FreeDirSector(), sector), LoadTable();

  for (int i = 0; i < SECTOR_SIZE / sizeof(FileEntry); i++)
    if (!entry[i].filename[0]) {
      OsCopy(entry[i].filename, filename, 64);
      entry[i].first_sector = FreeSector(), entry[i].size = size,
      entry[i].next_entry_sector = 0xFFFF;
      WriteSector(FreeDirSector(), sector);
      break;
    }

  for (unsigned short s = entry->first_sector; size > 0;
       s = FreeSector()) {
    WriteSector(s, buffer), buffer += SECTOR_SIZE, size -= SECTOR_SIZE;
    Table[s] = FreeSector();
  }
  Table[FreeSector()] = (unsigned char)0xFFFF,
  WriteSector(2, Table);
  return 0;
}

unsigned short FreeDirSector() {
  unsigned char buffer[SECTOR_SIZE];
  for (unsigned short sector = 1; sector != 0xFFFF;) {
    ReadSector(sector, buffer);
    FileEntry *entry = (FileEntry *)buffer;
    if (entry->next_entry_sector == 0xFFFF)
      return entry->next_entry_sector = FreeSector(),
             WriteSector(sector, buffer), FreeSector();
    sector = entry->next_entry_sector;
  }
  return FreeSector();
}

unsigned short FreeSector() {
  for (unsigned short i = 3; i < 4096; i++)
    if (!Table[i])
      return i;
  return 0xFFFF;
}

void ReadSector(unsigned int lba, unsigned char *buffer) {
  asm volatile("int $0x13" ::"a"(0x02), "b"(0x01), "c"(lba / 18),
               "d"((lba % 18) + 1), "D"(buffer));
}
void WriteSector(unsigned int lba, unsigned char *buffer) {
  asm volatile("int $0x13" ::"a"(0x03), "b"(0x01), "c"(lba / 18),
               "d"((lba % 18) + 1), "D"(buffer));
}
