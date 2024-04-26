#ifndef roster_h
#define roster_h

#include "student.h"
#include <vector>

using namespace std;

class Roster {
   public:
      ~Roster();
      void add(string studentID, string firstName, string lastName, string emailAddress, int age, int daysInCourse1, int daysInCourse2, int daysInCourse3, DegreeProgram degreeProgram);
      void addUnparsedStudent(string studentData);
      void remove(string studentID);
      void printAll();
      void printAverageDaysInCourse(string studentID);
      void printInvalidEmails();
      void printByDegreeProgram(DegreeProgram degreeProgram);
      vector<Student*> classRosterArray;
   private:
      bool hasStudent(string studentID);
      unsigned int getStudentIndex(string studentID);
      bool isValidEmail(string emailAddress);
};

#endif