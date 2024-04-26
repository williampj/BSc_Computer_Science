#include "student.h"
#include <vector>
#include <string>
#include <iostream>

using namespace std;

/* Constructor using setters to initialize student */
Student::Student(
   string studentID, 
   string firstName, 
   string lastName, 
   string emailAddress, 
   int age,
   int daysInCourse1, 
   int daysInCourse2, 
   int daysInCourse3, 
   DegreeProgram degreeProgram
   ) {
   setDaysToCompleteCourse(daysInCourse1, daysInCourse2, daysInCourse3);
   setStudentID(studentID);
   setFirstName(firstName);
   setLastName(lastName);
   setEmailAddress(emailAddress);
   setAge(age);
   setDegreeProgram(degreeProgram);
}

void Student::print() {
   cout << getStudentID() << "\t";
   cout << "First Name: " << getFirstName() << "\t";
   cout << "Last Name: " << getLastName() << "\t";
   cout << "Age: " << getAge() << "\t\t";
   cout << "daysInCourse: {" << getNumDaysToCompleteCourses()[0] << ", ";
   cout << getNumDaysToCompleteCourses()[1] << ", ";
   cout << getNumDaysToCompleteCourses()[2] << "} " << "\t";
   cout << "Degree Program: " << mapDegreeEnumToString(getDegreeProgram()) << "." << endl;
}

/* ------ Setters ------ */

void Student::setStudentID(string studentID) {
   this->studentID = studentID;
}

void Student::setFirstName(string firstName) {
   this->firstName = firstName;
}

void Student::setLastName(string lastName) {
   this->lastName = lastName;
}

void Student::setEmailAddress(string emailAddress) {
   this->emailAddress = emailAddress;
}

void Student::setAge(int age) {
   this->age = age;
}

void Student::setDaysToCompleteCourse(int daysInCourse1, int daysInCourse2, int daysInCourse3) {
   vector<int> daysToCompleteCourses = {daysInCourse1, daysInCourse2, daysInCourse3};
   this->daysToCompleteCourses = daysToCompleteCourses;
}

void Student::setDegreeProgram(DegreeProgram degreeProgram) {
   this->degreeProgram = degreeProgram;
}

/* ------ Public Helper Methods ------ */

string Student::mapDegreeEnumToString(DegreeProgram degreeProgram) {
  if (degreeProgram == 0) {
    return "SECURITY";
  } 
  else if (degreeProgram == 1) {
    return "NETWORK";
  } else {
    return "SOFTWARE";
  }
}

DegreeProgram Student::mapDegreeStringToEnum(string degreeProgramString) {
   if (degreeProgramString == "SOFTWARE") {
      return SOFTWARE;
   }
   else if (degreeProgramString == "NETWORK") {
      return NETWORK;
   } 
   else {
      return SECURITY;
   }
}