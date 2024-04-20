/*
 * Calculator.cpp
 *
 *  Date: 03/12/2023
 *  Author: Jarrett Henkel
 */

#include <iostream>

using namespace std;

int main()  //changed void to int
{
	double op1, op2;// removed char statement and changed int to double
	char operation;
	char answer = 'Y'; // char needs '
	while ((answer != 'q') && (answer != 'Q')) // changed to && to stop for either q or Q as well as continuing for anything other than those two. (n and N included)
	{
		cout << "Enter expression" << endl;
		cin >> op1 >> operation >> op2; //Reversed op1 and op 2 for simplicity
		if (operation == '+') // Change ' to " as well as removing semicolon and adding bracket.
		{
			cout << op1 << " + " << op2 << " = " << op1 + op2 << endl; // >> to <<
		}
		if (operation == '-') //removing semicolon and adding bracket.
		{
			cout << op1 << " - " << op2 << " = " << op1 - op2 << endl;// >> to <<
		}
		if (operation == '*')
		{
			cout << op1 << " * " << op2 << " = " << op1 * op2 << endl; // >> to <<. - to *
		}
		if (operation == '/') // added bracket
		{
			cout << op1 << " / " << op2 << " = " << op1 / op2 << endl; // >> to <<. - to /
		}

		cout << "Press anything to enter another expression. Press q or Q to quit." << endl;
		cin >> answer;
	}
	cout << "Program Finished" << endl; // added Program Finisher
}