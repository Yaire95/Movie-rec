/** @file RecommenderSystem.cpp
* @author  Yair Escott <yair.95@gmail.com>
*
* @brief the system for recommending movies
*/

// ------------------------------ includes ------------------------------
#include "RecommenderSystem.h"

using namespace std;
// ------------------------------ functions -----------------------------

/**
 * @brief loads the data from the files
 * @param moviesAttributesFilePath file 1
 * @param userRanksFilePath file 2
 * @return 0 if succeeds and -1 if not
 */
int RecommenderSystem::loadData(const string& moviesAttributesFilePath,
                                const string& userRanksFilePath)
{
    if(_readMoviesAtt(moviesAttributesFilePath) == EXIT_FAILURE || _readUserRanks
       (userRanksFilePath) == EXIT_FAILURE)
    {
        cout << FILEPATHERROR << moviesAttributesFilePath;
        return -1;
    }
    return 0;
}

/**
 * @brief reads the users rating from the file
 * @param userRanksFilePath the filepath
 */
int RecommenderSystem::_readUserRanks(const string& userRanksFilePath)
{
    string line;
    string word;
    int customerCounter = 0;
    vector<string> movieArr;
    ifstream usersFile(userRanksFilePath);
    if(!usersFile.is_open())
    {
        return EXIT_FAILURE;
    }
    getline(usersFile, line);
    stringstream iss(line);
    int movieCounter = 0;
    while (iss >> word)
    {
        this->_customerSecond.push_back(word);
        movieArr.push_back(word);
        movieCounter ++;
    }
    while (getline(usersFile, line))
    {
        stringstream iss2(line);
        string userName;
        int ratingCounter = 0;
        double theAvg = 0;
        vector<pair<string, double>> customerPair;
        vector<double> customerPairNormal;
        iss2 >> userName;
        while (iss2 >> word)
        {
            if(word == "NA")
            {
                customerPair.emplace_back(movieArr[ratingCounter], 0);
            }
            else
            {
                customerPair.emplace_back(movieArr[ratingCounter], stoi(word));
                theAvg += stoi(word);
                ratingCounter ++;
            }
        }
        this->_customerRatingsVec.push_back(userName);
        this->_customerRatings.insert({userName, customerPair});
        theAvg = theAvg / ratingCounter;
        for(auto i : customerPair)
        {
            if(i.second != 0)
            {
                customerPairNormal.push_back(i.second - theAvg);
            }
            else
            {
                customerPairNormal.push_back(0.0);
            }
        }
        this->_customerRatingsNormalVec.insert({userName, customerPairNormal});
        customerCounter ++;
    }
    this->_numberOfCustomers = customerCounter;
    return EXIT_SUCCESS;
}

/**
 * @brief reads the movies attributes from the file
 * @param moviesAttributesFilePath the path to the file
 */
int RecommenderSystem::_readMoviesAtt(const string& moviesAttributesFilePath)
{
    string line;
    ifstream moviesFile(moviesAttributesFilePath);
    if(!moviesFile.is_open())
    {
        return EXIT_FAILURE;
    }
    string word;
    int movieCounter = 0;
    int attributeCounter = 0;
    while (getline(moviesFile, line))
    {
        vector<double> attributeVec;
        string movieName;
        stringstream iss(line);
        iss >> word;
        movieName = word;
        double normal = 0.0;
        while (iss >> word)
        {
            double wordl = stoi(word);
            normal += pow(wordl, 2.0);
            attributeVec.push_back(wordl);
            attributeCounter ++;
        }
        this->_movieRatingsNormal.insert({movieName, sqrt(normal)});
        this->_movieRatingsVec.push_back(movieName);
        this->_movieRatings.insert({movieName, attributeVec});
        movieCounter ++;
    }
    this->_numberOfEvaluations = attributeCounter / movieCounter;
    this->_numberOfMovies = movieCounter;
    return EXIT_SUCCESS;
}

/**
 * @brief recommends a movie to the user by content
 * @param userName the username
 * @return the recommended movie
 */
string RecommenderSystem::recommendByContent(const string& userName) const
{
    if(!_findCustomer(userName))
    {
        return USERNOTFOUND;
    }
    vector<double> userVec(_numberOfEvaluations, 0);
    for (int j = 0; j < _numberOfEvaluations; j++)
    {
        for(int i = 0; i < this->_numberOfMovies; i++)
        {
            userVec[j] += ((this->_customerRatingsNormalVec.at(userName)[i] *
                    this->_movieRatings.at(_customerSecond[i])[j]));
        }
    }
    string recommend;
    double rank = 0.0;
    int counter = 0;
    for(auto i : this->_customerSecond)
    {
        if (this->_customerRatings.at(userName)[counter].second == 0)
        {
            if (recommend.empty())
            {
                rank = _multiply(this->_movieRatings.at(i), userVec) /
                        (_normFinder(userVec) * _normFinder(this->_movieRatings.at(i)));
                recommend = i;
            }
            else
            {
                double possible = _multiply(this->_movieRatings.at(i), userVec) /
                        (_normFinder(userVec) * _normFinder(this->_movieRatings.at(i)));
                if (possible > rank)
                {
                    rank = possible;
                    recommend = i;
                }
            }
        }
        counter++;
    }
    return recommend;
}

/**
 * @brief predicts the score for any movie for a certain user
 * @param movieName the movie to check
 * @param userName the user to check
 * @param k the number of movies to take into account
 * @return the movie's score
 */
double RecommenderSystem::predictMovieScoreForUser(const string& movieName, const string&
                                                   userName, int k) const
{
    vector<pair<double, int>> kVec;
    int counter = 0;
    for(int i = 0; i < this->_numberOfMovies; i++)
    {
        if (_customerRatings.at(userName)[counter].second != 0)
        {
            double rank = _multiply(this->_movieRatings.at(movieName), this->_movieRatings
                    .at(this->_customerSecond[i])) / (this->_movieRatingsNormal.at(movieName) *
                    this->_movieRatingsNormal.at(this->_customerSecond[i]));
            kVec.emplace_back(rank, i);
        }
        counter ++;
    }
    stable_sort(kVec.begin(), kVec.end(), [](const pair<double, int>& p1, const pair<double,
            int>& p2)
    {
        return p1.first > p2.first;
    });
    int counter2 = 0;
    double predictRating = 0.0;
    double devider = 0.0;
    for(auto l : kVec)
    {
        if(counter2 == k)
        {
            break;
        }
        devider += l.first;
        predictRating += l.first * this->_customerRatings.at(userName)[l.second].second;
        counter2 ++;
    }
    return predictRating / devider;
}

/**
 * @brief recommends a movie based on our prediction
 * @param userName the user
 * @param k the number of movies to tke into account
 * @return the movie recommended
 */
string RecommenderSystem::recommendByCF(const string& userName, int k) const
{
    if(!_findCustomer(userName))
    {
        return USERNOTFOUND;
    }
    else
    {
        bool flag = false;
        string movieBestFit;
        double bestFit = 0.0;
        for(int i = 0; i < this->_numberOfMovies; i++)
        {
            if (this->_customerRatings.at(userName)[i].second == 0)
            {
                double rank = predictMovieScoreForUser(this->_customerSecond[i],
                        userName, k);
                if (rank > bestFit || !flag)
                {
                    movieBestFit = this->_customerSecond[i];
                    bestFit = rank;
                    flag = true;
                }
            }
        }
        return movieBestFit;
    }
}

/**
 * @brief checks if the customer exists
 * @param customer the customer
 * @return true if yes and false if no
 */
bool RecommenderSystem::_findCustomer(const string& customer) const
{
    try
    {
        string test = this->_customerRatings.at(customer)[0].first;
        return true;
    }
    catch (exception &y)
    {
        return false;
    }
}

/**
 * @brief finde the norm of a vector
 * @param vec the vector
 * @return the norm
 */
double RecommenderSystem::_normFinder(const vector<double> vec) const
{
    double sum = 0.0;
    for(double i : vec)
    {
        sum += pow(i, 2.0);
    }
    return sqrt(sum);
}

/**
 * @brief multiplies two vectors
 * @param a vec 1
 * @param b vec 2
 * @return the multiplication
 */
double RecommenderSystem::_multiply(const vector<double> a, const vector<double> b) const
{
    double sum = 0.0;
    for(int i = 0; i < (int) a.size(); i++)
    {
        sum += a[i] * b[i];
    }
    return sum;
}