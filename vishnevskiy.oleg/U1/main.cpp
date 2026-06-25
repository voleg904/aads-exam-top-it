#include <iostream>
#include <fstream>
#include <sstream>
#include "ListImpl.hpp"

struct Person
{
  size_t id;
  std::string info;
};

int parseLine(std::istream& in, Person& person)
{
  std::string line;
  if (!std::getline(in, line))
  {
    return 1;
  }
  size_t start = line.find_first_not_of(" \t");
  if (start == std::string::npos)
  {
    person.id = 0;
    person.info = "";
    return 2;
  }
  line = line.substr(start);

  size_t end = line.find_last_not_of(" \t");
  if (end != std::string::npos)
  {
    line = line.substr(0, end + 1);
  }
  size_t delimiterPos = line.find_first_of(" \t");

  if (delimiterPos == std::string::npos)
  {
    person.id = 0;
    person.info = "";
    return 2;
  }
  std::string idStr = line.substr(0, delimiterPos);
  bool isNumber = true;
  for (char c : idStr)
  {
    if (!std::isdigit(c))
    {
      isNumber = false;
      break;
    }
  }

  if (!isNumber || idStr.empty())
  {
    person.id = 0;
    person.info = "";
    return 2;
  }
  try
  {
    person.id = std::stoull(idStr);
  }
  catch (const std::exception& e)
  {
    person.id = 0;
    person.info = "";
    return 2;
  }
  size_t infoStart = line.find_first_not_of(" \t", delimiterPos + 1);
  if (infoStart != std::string::npos)
  {
    person.info = line.substr(infoStart);
    if (person.info.find_first_not_of(" \t\"") == std::string::npos)
    {
      person.id = 0;
      person.info = "";
      return 2;
    }
  }
  else
  {
    person.id = 0;
    person.info = "";
    return 2;
  }
  return 0;
}

void setNames(std::string arg, std::string& inName, std::string&outName, bool& hasIn, bool& hasOut)
{
  std::string prefix = arg.substr(0, 3);
  std::string filename = arg.substr(3);
  std::string prefix2 = arg.substr(0, 4);
  std::string filename2 = arg.substr(4);
  if (filename == "" || (filename == ":" && prefix == "out"))
  {
    throw std::logic_error("empty name");
  }
  if (prefix == "in:")
  {
    if (hasIn)
    {
      throw std::logic_error("2 ins");
    }
    inName = filename;
    hasIn = true;
  }
  else if (prefix2 == "out:")
  {
    if (hasOut)
    {
      throw std::logic_error("2 outs");
    }
    outName = filename2;
    hasOut = true;
  }
  else
  {
    throw std::logic_error("Bad args");
  }
}

bool idExists(vishnevskiy::List<Person>* head, size_t id)
{
  vishnevskiy::LIter<Person> it(head);
  while (it.curr)
  {
    if (it.value().id == id)
    {
      return true;
    }
    ++it;
  }
  return false;
}

void addPerson(vishnevskiy::List<Person>*& head, const Person& person)
{
  vishnevskiy::List<Person>* newNode = new vishnevskiy::List<Person>(person, nullptr);
  if (!head)
  {
    head = newNode;
  }
  else
  {
    vishnevskiy::LIter<Person> it(head);
    while (it.curr && it.curr->next)
    {
      ++it;
    }
    it.curr->next = newNode;
  }
}

void clearList(vishnevskiy::List<Person>* head)
{
  if (!head)
  {
    return;
  }
  if (head->next)
  {
    clearList(head->next);
    head->next = nullptr;
  }
  delete head;
}

int main(int argc, char *argv[])
{
  std::string inName = "";
  std::string outName = "";
  bool hasIn = false;
  bool hasOut = false;
  if (argc > 3)
  {
    return 1;
  }

  for (int i = 1; i < argc; ++i)
  {
    std::string arg = argv[i];
    try
    {
      setNames(arg, inName, outName, hasIn, hasOut);
    }
    catch (const std::logic_error& e)
    {
      return 1;
    }
  }

  std::ifstream inputFile;
  std::istream* inPtr = &std::cin;
  if (hasIn)
  {
    inputFile.open(inName);
    if (!inputFile.is_open())
    {
      return 2;
    }
    inPtr = &inputFile;
  }

  std::ofstream outputFile;
  std::ostream* outPtr = &std::cout;
  if (hasOut)
  {
    outputFile.open(outName);
    if (!outputFile.is_open())
    {
      return 2;
    }
    outPtr = &outputFile;
  }

  vishnevskiy::List<Person>* personList = nullptr;

  Person person;
  int parseResult;

  while ((parseResult = parseLine(*inPtr, person)) != 1)
  {
    if (parseResult == 2)
    {
      continue;
    }
    if (!idExists(personList, person.id))
    {
      addPerson(personList, person);
    }
  }

  vishnevskiy::LIter<Person> it(personList);
  while (it.curr)
  {
    *outPtr << it.value().id << " " << it.value().info << std::endl;
    ++it;
  }

  clearList(personList);
  personList = nullptr;
  if (inputFile.is_open()) inputFile.close();
  if (outputFile.is_open()) outputFile.close();
  return 0;
}
