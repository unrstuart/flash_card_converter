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

std::vector<std::string> GetSpanText(const std::string& str) {
  std::string::size_type index = 0;

  std::vector<std::string> ret;
  while ((index = str.find("span class=\"", index)) != std::string::npos) {
    auto start = str.find(">", index) + 1;
    auto end = str.find("<", start);
    ret.push_back(str.substr(start, end - start));
    index = end;
  }
  return ret;
}

std::string Join(const std::vector<std::string>& vec, const std::string sep) {
  bool first = true;
  std::string ret = "";
  for (const auto& s : vec) {
    if (!first) ret += sep;
    first = false;
    ret += s;
  }
  return ret;
}

bool ExtractEnglish1(const std::string& contents, std::string* ret) {
  auto begin = contents.find("span lang=\"en\"");
  if (begin == contents.npos) return false;
  auto end = contents.find("\n</span>", begin);
  const std::string substr = contents.substr(begin, end - begin);
  std::vector<std::string> trans = GetSpanText(substr);
  for (std::string& s : trans) {
    if (s.substr(0, 3) != "to " && s.substr(0, 3) != "To ") {
      s = "to " + s;
    }
  }
  *ret = Join(trans, ", ");
  return !trans.empty();
}

std::string ExtractEnglish(const std::string& contents,
                           const std::string& verb) {
  std::string ret;
  if (ExtractEnglish1(contents, &ret)) return ret;
  std::cerr << "Couldn't find english span for " << verb << std::endl;

  return "";
}

std::vector<std::string> SplitTableData(std::string str) {
  str = str.substr(0, str.find("\n"));
  auto index = str.find("<td>");
  std::vector<std::string> ret;
  while ((index = str.find("<td>", index)) != std::string::npos) {
    ret.push_back(str.substr(index + 4, str.find("</td>", index) - index - 4));
    index += 4 + ret.back().size();
  }
  return ret;
}

void ExtractConjugations(const std::string& contents, Verb* verb) {
  auto index = contents.find(R"(
<th scope="col">Present</th>
<th scope="col">Imperfect</th>
<th scope="col">Subjunctive I</th>
<th scope="col">Subjunctive II</th>
<th scope="col">Imperative</th>)");
  if (index == std::string::npos) {
    std::cerr << "Couldn't extract conjugations for " << verb->infinitive
              << std::endl;
    return;
  }
  index = contents.find("<tr><td>du ", index);
  if (index == std::string::npos) {
    std::cerr << "Couldn't further extract conjugations for "
              << verb->infinitive;
    return;
  }
  const std::vector<std::string> tokens =
      SplitTableData(contents.substr(index));
  verb->present_du_form = tokens[1];
  verb->preterite = tokens[2];
  verb->konj_1 = tokens[3];
  verb->konj_2 = tokens[4];
  verb->imperative = tokens[5];
}

std::string ExtractParticiple(const std::string& contents,
                              const std::string& verb) {
  auto index = contents.find("<b>Participle II</b>:");
  if (index == contents.npos) {
    std::cerr << "couldn't extract participle for " << verb << std::endl;
    return "";
  }
  index = contents.find("\n", index) + 1;
  auto next = contents.find("\n", index);
  return contents.substr(index, next - index);
}

Verb ScrapeVerb(const std::string& v) {
  static bool scrape = false;
  char cmd[2000];
  std::string file = v;
  if (file.substr(0, 5) == "sich ") file = file.substr(5);
  if (scrape) {
    sprintf(cmd,
            "curl 'https://www.verbformen.com/conjugation/?w=%s' > /tmp/%s.htm",
            file.c_str(), file.c_str());
    std::cerr << "Executing '" << cmd << "'... " << std::endl;
    if (system(cmd) != 0) {
      std::cerr << " error" << std::endl;
    } else {
      std::cerr << " done" << std::endl;
    }
  }
  sprintf(cmd, "/tmp/%s.htm", file.c_str());
  std::string contents = ReadFile(cmd);

  Verb verb;
  verb.infinitive = v;
  verb.english = ExtractEnglish(contents, file);
  ExtractConjugations(contents, &verb);
  verb.participle = ExtractParticiple(contents, v);

  return verb;
}

int main(int argc, char** argv) {
  std::vector<Verb> verbs;
  for (const std::string& verb : ReadLines(argv[1])) {
    verbs.push_back(ScrapeVerb(verb));
  }
  for (const Verb& verb : verbs) {
    std::cout << verb << "\n";
  }

  return 0;
}

