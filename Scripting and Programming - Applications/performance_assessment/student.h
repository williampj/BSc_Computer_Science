#ifndef student_h
#define student_h

#include <string>
#include <vector>
#include "degree.h"
using namespace std;

class Student {
   public:
      Student(
         string studentID, 
         string firstName, 
         string lastName, 
         string emailAddress, 
         int age,
         int daysInCourse1, 
         int daysInCourse2, 
         int daysInCourse3, 
         DegreeProgram degreeProgram
      );

      void print();

      string getStudentID() { return studentID; }
      void setStudentID(string studentID);
      
      string getFirstName() { return firstName; }
      void setFirstName(string firstName);
      
      string getLastName() { return lastName; }
      void setLastName(string lastName);

      string getEmailAddress() { return emailAddress; }
      void setEmailAddress(string emailAddress);
      
      int getAge() { return age; }
      void setAge(int age);

      vector<int> getNumDaysToCompleteCourses() { return daysToCompleteCourses; }
      void setDaysToCompleteCourse(int daysInCourse1, int daysInCourse2, int daysInCourse3);

      DegreeProgram getDegreeProgram() { return degreeProgram; }
      void setDegreeProgram(DegreeProgram degreeProgram);

      static string mapDegreeEnumToString(DegreeProgram degreeProgram);
      static DegreeProgram mapDegreeStringToEnum(string degreeProgramString);
   
   private:
      string studentID;
      string firstName;
      string lastName;
      string emailAddress;
      vector<int> daysToCompleteCourses;
      int age;
      DegreeProgram degreeProgram;
};

#endif