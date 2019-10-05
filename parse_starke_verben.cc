// Converts a list of strong german verbs to a TSV file. The file
// 'starke_verben' should have been included in this repository.
//
// Usage: a.out starke_verben > out.tsv

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>

struct Verb {
  std::string infinitive;
  std::string form;
  std::string english;
  std::string preterite;
  std::string participle;
  std::string imperative;
  std::string present_du_form;
  std::string konj_1;
  std::string konj_2;
};

std::string Trim(const unsigned char* str) {
  int s = 0;
  while (str[s] != '\0' && str[s] <= ' ') ++s;
  int e = s;
  while (str[e] != '\0') ++e;
  while (e > s && str[e] <= ' ') --e;
  std::string ret;
  ret.resize(e - s + 1);
  memcpy(&ret[0], str, e - s + 1);
  return ret;
}

std::string Trim(const std::string& str) {
  int s = 0;
  while (s < str.size() && static_cast<unsigned char>(str[s]) <= ' ') ++s;
  int e = str.size() - 1;
  while (e > s && static_cast<unsigned char>(str[e]) <= ' ') --e;
  return str.substr(s, e - s + 1);
}

std::string FindNextToken(const std::string& str) {
  if (str.empty()) return str;
  int e = 0;
  while (e < str.size() && static_cast<unsigned char>(str[e]) <= ' ') ++e;
  if (e == str.size()) return str;
  while (e < str.size() && static_cast<unsigned char>(str[e]) > ' ') ++e;
  return str.substr(0, e + 1);
}

std::vector<std::string> Tokenize(std::string str) {
  std::vector<std::string> ret;
  while (!str.empty()) {
    std::string token = FindNextToken(str);
    str = Trim(str.substr(token.size()));
    token = Trim(token);
    if (!token.empty()) ret.push_back(Trim(token));
  }
  return ret;
}

template <typename Iter>
std::string MakeString(Iter begin, Iter end) {
  std::string str = "";
  while (begin < end) {
    if (!str.empty()) str += " ";
    str += *(begin++);
  }
  return str;
}

std::ostream& operator<<(std::ostream& out, const Verb& verb) {
  return out << verb.infinitive << "\t" << verb.form << "\t" << verb.english
             << "\t" << verb.preterite << "\t" << verb.imperative << "\t"
             << verb.present_du_form << "\t" << verb.konj_1 << "\t"
             << verb.konj_2;
}

int main(int argc, char** argv) {
  FILE* fp = fopen(argv[1], "r");
  char buf[1024];
  unsigned char* p = reinterpret_cast<unsigned char*>(buf);
  std::vector<Verb> verbs;
  while (fgets(buf, 1023, fp) != 0) {
    std::vector<std::string> tokens = Tokenize(Trim(p));
    Verb verb;
    verb.infinitive = tokens[0];
    tokens.erase(tokens.begin(), tokens.begin() + 1);
    verb.form = tokens[0];
    tokens.erase(tokens.begin(), tokens.begin() + 1);
    verb.konj_2 = MakeString(tokens.end() - 4, tokens.end());
    tokens.erase(tokens.end() - 4, tokens.end());
    verb.konj_1 = MakeString(tokens.end() - 3, tokens.end());
    tokens.erase(tokens.end() - 3, tokens.end());
    if (tokens.back() == "ab") {
      verb.present_du_form = MakeString(tokens.end() - 2, tokens.end());
      tokens.erase(tokens.end() - 2, tokens.end());
    } else {
      verb.present_du_form = MakeString(tokens.end() - 1, tokens.end());
      tokens.erase(tokens.end() - 1, tokens.end());
    }
    if (tokens.back() == "ab!") {
      verb.imperative = MakeString(tokens.end() - 2, tokens.end());
      tokens.erase(tokens.end() - 2, tokens.end());
    } else {
      verb.imperative = MakeString(tokens.end() - 1, tokens.end());
      tokens.erase(tokens.end() - 1, tokens.end());
    }
    verb.participle = tokens.back();
    tokens.pop_back();
    if (tokens.back() == "ab") {
      verb.preterite = MakeString(tokens.end() - 2, tokens.end());
      tokens.erase(tokens.end() - 2, tokens.end());
    } else {
      verb.preterite = MakeString(tokens.end() - 1, tokens.end());
      tokens.erase(tokens.end() - 1, tokens.end());
    }
    verb.english = MakeString(tokens.begin(), tokens.end());
    std::cout << verb << std::endl;
  }
  return 0;
}
