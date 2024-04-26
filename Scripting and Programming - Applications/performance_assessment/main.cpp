#include <iostream>
#include "student.h"
#include "roster.h"

using namespace std;

const string studentData[] = {
   "A1,John,Smith,John1989@gm ail.com,20,30,35,40,SECURITY",
   "A2,Suzan,Erickson,Erickson_1990@gmailcom,19,50,30,40,NETWORK",
   "A3,Jack,Napoli,The_lawyer99yahoo.com,19,20,40,33,SOFTWARE",
   "A4,Erin,Black,Erin.black@comcast.net,22,50,58,40,SECURITY",
   "A5,William,Jackson,wjac104@wgu.edu,38,10,20,30,SOFTWARE"
};

/* WGU course title, programming language, student ID and name */
void printCourseInfo() {
   cout << "\nCourse Title: Scripting and Programming - Applications — C867" << endl;
   cout << "Programming Language: C++" << endl;
   cout << "WGU Student ID: 002982231" << endl;
   cout << "Name: William Jackson" << endl << endl;
}

int main() {
   Student* student;
   Roster* classRoster = new Roster; 
   string studentID;
   unsigned int i; 
   
   printCourseInfo();
  
   for (string student : studentData) {
      classRoster->addUnparsedStudent(student);
   }

   classRoster->printAll();
   classRoster->printInvalidEmails();

   for (i = 0; i < classRoster->classRosterArray.size(); ++i) {
      studentID = classRoster->classRosterArray.at(i)->getStudentID();
      classRoster->printAverageDaysInCourse(studentID);
   }

   classRoster->printByDegreeProgram(SOFTWARE);

   classRoster->remove("A3");

   classRoster->printAll();

   classRoster->remove("A3");

   delete classRoster;

   return 0;
}