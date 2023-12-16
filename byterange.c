/* byterange, grab a range of bytes from a file
   Copyright (C) 2023  Nikolaos Chatzikonstantinou <nchatz314@gmail.com>

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */
#include <config.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void usage(void) {
  fprintf(stderr, "Usage: %s file begin end\n\n", PACKAGE_NAME);
  fprintf(stderr,
          "Reads (END - BEGIN) bytes from the offset BEGIN of FILE "
          "and outputs\nto standard output.\n\n");
  fprintf(stderr, "Arguments:\n\n");
  fprintf(stderr, "  FILE\t\tThe file to grab bytes from.\n");
  fprintf(stderr, "  BEGIN\t\tThe beginning offset to grab bytes "
                  "from (inclusive.)\n");
  fprintf(stderr, "  END\t\tThe ending offset to grab bytes from "
                  "(exclusive.)\n\n");
  fprintf(stderr, "Report bugs to <%s>.\n", PACKAGE_BUGREPORT);
}

static void version(void) {
  printf("%s\n", PACKAGE_STRING);
}

int main(int argc, char **argv) {
  FILE *fp;
  size_t n;
  long total, begin, end;
  char *err, buf[BUFSIZ];
  if(argc != 4) {
    if(argc == 2 &&
       (!strcmp(argv[1], "-v") ||
        !strcmp(argv[1], "--version"))) {
      version();
      return EXIT_SUCCESS;
    }
    usage();
    return EXIT_FAILURE;
  }
  begin = strtol(argv[2], &err, 10);
  if(err == argv[2]) {
    fprintf(stderr, "BEGIN is not an integer.\n");
    return EXIT_FAILURE;
  }
  end = strtol(argv[3], &err, 10);
  if(err == argv[3]) {
    fprintf(stderr, "END is not an integer.\n");
    return EXIT_FAILURE;
  }
  if(begin < 0) {
    fprintf(stderr, "Invalid range: BEGIN should be non-negative.\n");
    return EXIT_FAILURE;
  }
  if(end <= begin) {
    fprintf(stderr, "Invalid range: END should be larger than "
                    "BEGINNING.\n");
    return EXIT_FAILURE;
  }
  if((fp = fopen(argv[1], "rb")) == NULL) {
    perror("fopen");
    return EXIT_FAILURE;
  }
  if(setvbuf(fp, NULL, _IONBF, 0) != 0) {
    perror("setvbuf");
    fclose(fp);
    return EXIT_FAILURE;
  }
  if(fseek(fp, begin, SEEK_SET) != 0) {
    perror("fseek");
    fclose(fp);
    return EXIT_FAILURE;
  }
  for(total = end - begin; !feof(fp) && total >= BUFSIZ; total -= n) {
    n = fread(buf, 1, sizeof buf, fp);
    if(n != sizeof buf && !feof(fp)) {
      fclose(fp);
      fwrite(buf, 1, n, stdout);
      return EXIT_FAILURE;
    }
    if(fwrite(buf, 1, n, stdout) != n) {
      fclose(fp);
      return EXIT_FAILURE;
    }
  }
  if(!feof(fp) && total != 0) {
    n = fread(buf, 1, (size_t)total, fp);
    if(n != total && !feof(fp)) {
      fclose(fp);
      fwrite(buf, 1, n, stdout);
      return EXIT_FAILURE;
    }
    fclose(fp);
    if(fwrite(buf, 1, n, stdout) != n) {
      return EXIT_FAILURE;
    }
  }
  return EXIT_SUCCESS;
}
