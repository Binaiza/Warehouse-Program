// Warehouse Program.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include<cstdlib>
#include<ctime>
#include <vector>
#include <memory>
#include <string>
#include <map>
#include<algorithm>
using namespace std;

struct changelog
{
    string description;
    time_t timestamp;
    changelog(const string&desc):description(desc),timestamp(time(nullptr)){}
};

class Employee
{
public:
    string name;
    int id;
    Employee(string name,int id):name(name),id(id){}
};

class position :public
    enable_shared_from_this<position>
{
public:
    string title;
    shared_ptr<Employee>employee;
    vector<shared_ptr<position>>subordinates;
    weak_ptr<position>parent;
    position(string title):title(title){}

    void assignemployee(shared_ptr<Employee>emp)
    {
        employee = emp;
    }

    void addsubordinate(shared_ptr<position>sub)
    {
        sub->parent = shared_from_this();
        subordinates.push_back(sub);
    }

    void print(int level = 0)const { cout << string(level * 4, ' ') << " " << title;
    if (employee)
        cout << endl;
    for (const auto& sub : subordinates)
        sub->print(level + 1);
    }
};

class orgchartmanager
{
private:
    shared_ptr<position>root;
    vector<changelog>history;
    map<string, shared_ptr<position>>titlemap;

    void removefrommap(shared_ptr<position>pos)
    {
        for (auto& sub : pos->subordinates)
        {
            removefrommap(sub);
        }
        titlemap.erase(pos->title);
    }

public:
    orgchartmanager(const string& roottitle)
    {
        root = make_shared<position>(roottitle);
        titlemap[roottitle] = root;
        history.emplace_back("Initialized organization with root:" + roottitle);
    }

    void addposition(const string& parenttitle, const string& newtitle)
    {
        if (titlemap.find(parenttitle) == titlemap.end())
        {
            cout << "Parent position not found.\n";
            return;
        }
        if (titlemap.find(newtitle) != titlemap.end())
        {
            cout << "A position with this title already exists.\n";
            return;
        }
        auto newpos = make_shared<position>(newtitle);
        titlemap[parenttitle]->addsubordinate(newpos);
        titlemap[newtitle] = newpos;
        history.emplace_back("Added position:" + newtitle + "under" + parenttitle);
    }

    void assignemployeetoposition(const string& title, const string& name, int id)
    {
        if (titlemap.find(title) == titlemap.end())
        {
            cout << "Position not found.\n";
            return;
        }
        auto emp = make_shared<Employee>(name, id);
        titlemap[title]->assignemployee(emp);
        history.emplace_back("Assigned employee" + name + "to position:" + title);
    }

    void removeposition(const string& title)
    {
        if (titlemap.find(title) == titlemap.end())
        {
            cout << "Position not found.\n";
            return;
        }
        if (title == root->title)
        {
            cout << "Cannot remove root position.\n";
            return;
        }
        auto pos = titlemap[title];
        auto parentptr = pos->parent.lock();
        if (!parentptr)
            return;

        parentptr->subordinates.erase(remove_if(parentptr->subordinates.begin(), parentptr->subordinates.end(), [&](shared_ptr<position>& p) {return p->title == title; }), parentptr->subordinates.end());
        removefrommap(pos);
        history.emplace_back("Remaved position:" + title);
    }

    void editpositiontitle(const string& oldtitle, const string& newtitle)
    {
        if (titlemap.find(oldtitle) == titlemap.end())
        {
            cout << "Original position not found.\n";
            return;
        }
        if (titlemap.find(newtitle) != titlemap.end())
        {
            cout << "A position with the new title already exists.\n";
            return;
        }
        auto pos = titlemap[oldtitle];
        pos->title = newtitle;
        titlemap.erase(oldtitle);
        titlemap[newtitle] = pos;
        history.emplace_back("Renamed position from" + oldtitle + "to" + newtitle);
    }

    void printchart()const
    {
        cout << "\nOrganizational Chart:\n";
        root->print();
    }

    void printhistory()const
    {
        cout << "\nChange History:\n";
        for (const auto& log : history)
        {
            char buffer[26];
#if defined(_MSC_VER)
            ctime_s(buffer, sizeof(buffer), &log.timestamp);
#else
            ctime_r(&log.description << "at" << buffer;);
#endif
            cout << "-" << log.description << "at" << buffer;

        }
    }
};


int main()
{
    string roottitle;
    cout << "Enter title for tHe root position(e.g,CEO):";
    getline(cin, roottitle);
    orgchartmanager org(roottitle);

    int choice;
    do {
        cout << "\n=====Organization Chart Menu=====\n";
        cout << "1.Add Position\n";
        cout << "2.Assign Employee to position\n";
        cout << "3.Edit Position Title\n";
        cout << "4.Remove position\n";
        cout << "5.Print Organizational Chart\n";
        cout << "6.View Change History\n";
        cout << "0.Exit\n";
        cout << "Enter your choice:";
        cin >> choice;
        cin.ignore();

        if (choice == 1)
        {
            string parent, newtitle;
            cout << "Enter Parent position title:";
            getline(cin, parent);
            cout << "Enter new position title:";
            getline(cin, newtitle);
            org.addposition(parent, newtitle);
        }
        else if (choice == 2)
        {
            string title, name;
            int id;
            cout << "Enter position title:";
            getline(cin, title);
            cout << "Enter employee name:";
            getline(cin, name);
            cout << "Enter employee ID:";
            cin >> id;
            cin.ignore();

            org.assignemployeetoposition(title, name, id);
        }
        else if (choice == 3)
        {
            string oldtitle, newtitle;
            cout << "Enter current position title:";
            getline(cin, oldtitle);
            cout << "Enter new position title:";
            getline(cin, newtitle);
            org.editpositiontitle(oldtitle, newtitle);
        }
        else if (choice == 4)
        {
            string title;
            cout << "Enter Position title to remove:";
            getline(cin, title);
            org.removeposition(title);
        }
        else if (choice == 5)
        {
            org.printchart();
        }
        else if (choice == 6)
        {
            org.printhistory();
        }
        else if (choice == 0)
        {
            cout << "Exiting program.\n";
        }
        else
        {
            cout << "Invalid choice.Please try again.\n";
        }
    } while (choice != 0);
    return 0;
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
