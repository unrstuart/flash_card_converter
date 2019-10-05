// Converts a CSV with double quotes to deal with commas to a TSV

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <iostream>
#include <string>
#include <vector>

#include "verb.h"

std::string Trim(const std::string& str) {
  int s = 0;
  while (s < str.size() && static_cast<unsigned char>(str[s]) <= ' ') ++s;
  int e = str.size() - 1;
  while (e > s && static_cast<unsigned char>(str[e]) <= ' ') --e;
  return str.substr(s, e - s + 1);
}

std::vector<std::string> ReadLines(const char* path) {
  FILE* fp = fopen(path, "r");
  std::vector<std::string> lines;
  char buf[1024];
  while (fgets(buf, 1023, fp) != 0) {
    const std::string line = Trim(buf);
    if (!line.empty()) lines.push_back(line);
  }
  fclose(fp);
  return lines;
}

std::string ReadFile(const char* path) {
  std::string ret;
  for (const std::string& line : ReadLines(path)) {
    ret += line + "\n";
  }
  return ret;
}

std::string FixLine(std::string line) {
  std::string::size_type index = 0;
  while ((index = line.find("<", index)) != line.npos) {
    auto index2 = line.find(">", index);
    line = line.substr(0, index) + " " + line.substr(index2 + 1);
    index = index + 1;
  }
  index = 0;
  while ((index = line.find("&nbsp;")) != line.npos) {
    line = line.substr(0, index) + " " + line.substr(index + 6);
  }

  return line;
}

int main(int argc, char** argv) {
  for (std::string line : ReadLines(argv[1])) {
    line = FixLine(line);
    auto index = line.find("\"");
    if (index == line.npos) {
      index = line.find(",");
      std::cout << line.substr(0, index) << "\t" << line.substr(index + 1)
                << "\n";
      continue;
    }
    if (index == 0) {
      index = line.find("\"", 1);
      std::cout << line.substr(1, index - 1) << "\t"
                << line.substr(line.find_last_of(",") + 1) << "\n";
      continue;
    }
    std::cout << line.substr(0, line.find(",")) << "\t";
    line = line.substr(line.find(",") + 2);
    std::cout << line.substr(0, line.size() - 1) << "\n";
  }

  return 0;
}

