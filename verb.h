#ifndef VERB_H__
#define VERB_H__

#include <iostream>
#include <string>

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

std::ostream& operator<<(std::ostream& out, const Verb& verb) {
  return out << verb.infinitive << "\t" << verb.form << "\t" << verb.english
             << "\t" << verb.preterite << "\t" << verb.participle << "\t"
             << verb.imperative << "\t" << verb.present_du_form << "\t"
             << verb.konj_1 << "\t" << verb.konj_2;
}

#endif  // VERB_H
