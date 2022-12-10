#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <tuple>
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/convex_hull_2.h>
typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
typedef K::Point_2 Point_2;

using namespace std;

void printVector(vector<string> vec)
{
    for (int i = 0; i < vec.size(); i++)
        cout << vec[i] << endl;
}
void printVector(vector<Point_2> vec)
{
    for (int i = 0; i < vec.size(); i++)
        cout << vec[i].x() << " " << vec[i].y() << endl;
}

namespace inputformat
{
    // String Tokenization Function. Default delimiter = tab character \t
    vector<string> tokenizeStringByDelimiter(string text, char delimiter = '\t')
    {        
        // Vector of string to save tokens
        vector<string> tokens;

        // stringstream class check1
        stringstream check1(text);

        string intermediate;

        // Tokenizing w.r.t. delimiter character
        while (getline(check1, intermediate, delimiter))
        {
            tokens.push_back(intermediate);
        }

        return tokens;
    }

    // Read File and append each line content to a vector
    vector<string> readInputAndConvertContentToVector(string filename)
    {
        vector<string> lines;

        // Create a text string, which is used to output the text file
        string myText;

        // Read from the text file
        ifstream MyReadFile(filename);

        // Use a while loop together with the getline() function to read the file line by line
        while (getline(MyReadFile, myText))
        {
            // Store content of each line
            lines.push_back(myText);
        }

        // Close the file
        MyReadFile.close();

        return lines;
    }

    vector< Point_2 > formatFileContentsToCGALPoints(string filename, int &initial_area)
    {
        vector< Point_2 > points;

        vector<string> fileContent = readInputAndConvertContentToVector(filename);
        for (int i = 0; i < fileContent.size(); i++)
        {
            if (fileContent[i].find('\t') != string::npos)
            {
                vector<string> tokenizedString = tokenizeStringByDelimiter(fileContent[i]);
                // erasing first element because it is just the serial number
                tokenizedString.erase(tokenizedString.begin());

                points.push_back(Point_2(stoi(tokenizedString[0]), stoi(tokenizedString[1])));
            }
            else {
                if (fileContent[i].find('{') != string::npos)
                {
                    // extract convex hull area
                    vector<string> tokenizedString = tokenizeStringByDelimiter(fileContent[i],' ');
                    string ar = tokenizedString[4];
                    ar.erase(0,1);
                    ar.erase(ar.length()-2,2);
                    initial_area = stoi(ar);
                    
                }
            }
        }

        return points;
    }

};
