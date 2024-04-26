#include <string>
#include <vector>
#include <iterator>
#include <sstream>
#include <iostream>
#include <iomanip>
#include "roster.h"
#include "student.h"
#include "degree.h"

using namespace std;

/* Destructor deallocates the students referenced in classRosterArray */
Roster::~Roster() {
   unsigned int i; 
   
   for (i = 0; i < classRosterArray.size(); ++i) {
      delete classRosterArray.at(i);
   }
   
   cout << "Class roster has been deleted." << endl << endl;
}

/* Adds a new student to the roster */
void Roster::add(
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
   Student* newStudent = new Student(
      studentID, 
      firstName, 
      lastName, 
      emailAddress, 
      age, 
      daysInCourse1, 
      daysInCourse2, 
      daysInCourse3, 
      degreeProgram
   );
   classRosterArray.push_back(newStudent);
} 

/* Parses comma-delimited student data to assign student variables */
void Roster::addUnparsedStudent(string studentData) {
   string studentID, firstName, lastName, emailAddress, degreeProgramString;
   int age, daysInCourse1, daysInCourse2, daysInCourse3;
   DegreeProgram degreeProgram;
   
   stringstream studentStream(studentData);
   vector<string> dataPoints; 
   string dataPoint;
   
   // streams each studentData chunk until the ',' delimiter to dataPoint string
   while(getline(studentStream, dataPoint, ',')) {
      dataPoints.push_back(dataPoint);  
   };
   
   degreeProgram = Student::mapDegreeStringToEnum(dataPoints.back());
   dataPoints.pop_back();

   daysInCourse3 = stoi(dataPoints.back());
   dataPoints.pop_back();

   daysInCourse2 = stoi(dataPoints.back());
   dataPoints.pop_back();

   daysInCourse1 = stoi(dataPoints.back());
   dataPoints.pop_back();

   age = stoi(dataPoints.back());
   dataPoints.pop_back();

   emailAddress = dataPoints.back();
   dataPoints.pop_back();

   lastName = dataPoints.back();
   dataPoints.pop_back();

   firstName = dataPoints.back();
   dataPoints.pop_back();

   studentID = dataPoints.back();
   dataPoints.pop_back();

   add(studentID, 
      firstName, 
      lastName, 
      emailAddress, 
      age, 
      daysInCourse1, 
      daysInCourse2, 
      daysInCourse3, 
      degreeProgram);
}

/* Removes students from the roster by student ID. 
   If the student ID does not exist, the function prints an error message 
   indicating that the student was not found. 
*/
void Roster::remove(string studentID) {
   if (hasStudent(studentID)) {
      // returns index for the student to be removed
      unsigned int studentIndex = getStudentIndex(studentID);
      vector<Student*>::iterator indexPointer = classRosterArray.begin();
      // advances the iterator to the index number to be removed
      advance(indexPointer, studentIndex);
      // removes student at that index
      classRosterArray.erase(indexPointer);
      cout << "StudentID: " << studentID << " has been removed from the roster." << endl << endl;
   } 
   else {
      cout << "ERROR: The student with the ID: " << studentID << " was not found in the roster." << endl << endl;
   }
}

/* Prints a complete tab-separated list of student data in the provided format: 
   A1 [tab] First Name: John [tab] Last Name: Smith [tab] Age: 20 [tab]daysInCourse: {35, 40, 55} Degree Program: Security. 
   The function loops through all the students in classRosterArray and 
   calls the print() function for each student.
*/
void Roster::printAll() {
   unsigned int i;

   cout << "The following students are currently on the student roster:" << endl;
   for (i = 0; i < classRosterArray.size(); ++i) {
      classRosterArray.at(i)->print();
   }

   cout << endl;
}

/* Prints a student’s average number of days in the three courses. 
   The student is identified by the studentID parameter. 
*/
void Roster::printAverageDaysInCourse(string studentID) {
   Student* currStudent = classRosterArray.at(getStudentIndex(studentID));
   vector<int> daysInCourses = currStudent->getNumDaysToCompleteCourses();
   int totalDays = 0;
   unsigned int i;

   for (i = 0; i < daysInCourses.size(); ++i) {
      totalDays += daysInCourses.at(i);
   }

   cout << "StudentID: " << studentID << " spent an average of " << fixed << setprecision(2) << totalDays / 3.0 << " days per course." << endl;
}

/* Verifies student email addresses and displays all invalid email addresses to the user. 
   Note: A valid email should include an at sign ('@') and period ('.') and should not include a space (' ').
*/
void Roster::printInvalidEmails() {
   vector<string> invalidEmails;
   string currentEmail;
   unsigned int i;

   for (i = 0; i < classRosterArray.size(); ++i) {
      currentEmail = classRosterArray.at(i)->getEmailAddress();
      if (!isValidEmail(currentEmail)) {
         invalidEmails.push_back(currentEmail);
      }
   }

   if (invalidEmails.size() > 0) {
      cout << "The following student emails are invalid: " << endl;
      for (i = 0; i < invalidEmails.size(); ++i) {
         cout << invalidEmails.at(i) << endl;
      }
   } else {
      cout << "There are no invalid emails in the student roster." << endl;
   }

   cout << endl;
}

/* Prints out student information for a degree program specified 
   by an enumerated type. 
*/
void Roster::printByDegreeProgram(DegreeProgram degreeProgram) {
   vector<Student*> degreeStudents;
   Student* currentStudent;

   unsigned int i;
   for (i = 0; i < classRosterArray.size(); ++i) {
      currentStudent = classRosterArray.at(i);
      if (currentStudent->getDegreeProgram() == degreeProgram) {
         degreeStudents.push_back(currentStudent);
      }
   }

   if (degreeStudents.size() > 0) {
      cout << "\nThe following students are pursuing a degree program in " << Student::mapDegreeEnumToString(degreeProgram) << ":" << endl;
      for (i = 0; i < degreeStudents.size(); ++i) {
         degreeStudents.at(i)->print();
      }
   } 
   else {
      cout << "There are no students in the roster pursuing a degree in " << degreeProgram << "." << endl;
   }

   cout << endl;
}

/* ------ Private Helper Functions ------ */

bool Roster::hasStudent(string StudentID) {
   unsigned int i; 
  
   for (i = 0; i < classRosterArray.size(); ++i) {
      Student* currStudent = classRosterArray.at(i);
      if (StudentID == currStudent->getStudentID()) {
         return true;
      }
   }
   
   return false;
}

unsigned int Roster::getStudentIndex(string studentID) {
   unsigned int i; 
  
   for (i = 0; i < classRosterArray.size(); ++i) {
      Student* currStudent = classRosterArray.at(i);
      if (studentID == currStudent->getStudentID()) {
         return i;
      }
   }

   return -1;
}

bool Roster::isValidEmail(string emailAddress) {
   bool hasAtSign = emailAddress.find("@") != string::npos;
   bool hasDot = emailAddress.find(".") != string::npos;
   bool hasSpace = emailAddress.find(" ") != string::npos;

   return hasAtSign && hasDot && !hasSpace;
}