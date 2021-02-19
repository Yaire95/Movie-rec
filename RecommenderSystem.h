/** @file RecommenderSystem.h
* @author  Yair Escott <yair.95@gmail.com>
*
* @brief the system for recommending movies
*/

// ------------------------------ includes ------------------------------
#ifndef EX5_RECOMMENDERSYSTEM_H
#define EX5_RECOMMENDERSYSTEM_H
#define USERNOTFOUND "USER NOT FOUND"
#define FILEPATHERROR "Unable to open file"
#include <string>
#include <vector>
#include <unordered_map>
#include <filesystem>
#include <iostream>
#include <cmath>
#include <fstream>
#include <algorithm>
#include <map>

// ------------------------------ the class ------------------------------

using namespace std;

/**
 * @brief the class RecommenderSystem
 */
class RecommenderSystem
{
public:
    /**
     * @brief loads the data from the files
     * @param moviesAttributesFilePath the file path for the movie's attributes
     * @param userRanksFilePath file path for the user ranks
     * @return 0 if succeeded -1 if not
     */
    int loadData(const string& moviesAttributesFilePath, const string& userRanksFilePath);
    /**
     * @brief gets the recommended movie
     * @param userName the username
     * @return the movie name if found and if not- returns "USER NOT FOUND"
     */
    string recommendByContent(const string& userName) const;
    /**
     * @brief gets the rating for the user for this movie
     * @param movieName the movie name
     * @param k the number of movies to take into account for the algorithm
     * @return -1 if name's do not exist and a number that is teh movie's rating
     */
    double predictMovieScoreForUser(const string& movieName, const string& userName, int k) const;
    /**
     * @brief finds the next movie the user should watch
     * @param userName the userr
     * @param k the number of movies to take into account for the algorithm
     * @return the movie to watch or  "USER NOT FOUND" if the user dosn't exist
     */
    string recommendByCF(const string& userName, int k) const;

private:
    /**
     * @brief a maps containing the movies and their ratings
     */
    unordered_map<string, vector<double>> _movieRatings;
    /**
     * @brief stors the normalized values of every movie
     */
    unordered_map<string, double> _movieRatingsNormal;
    /**
     * @brief a vector with the movie names to be able to keep the order
     */
    vector<string> _movieRatingsVec;
    /**
     * @brief a map containing the users and their ratings
     */
    unordered_map<string, vector<pair<string, double>>> _customerRatings;
    /**
     * @brief a map with the normalized values for each user
     */
    unordered_map<string, vector<double>> _customerRatingsNormalVec;
    /**
     * @brief  vector of the customer names for keeping order
     */
    vector<string> _customerRatingsVec;
    /**
     * @brief a vector of the movies rated by user for the order
     */
    vector<string> _customerSecond;
    /**
     * @brief iterates over _customer rating to see if the customer exists
     * @param customer the customer to find
     * @return true if found false if not
     */
    bool _findCustomer(const string& customer) const;
    /**
 * @brief finds the minimum rank in the vector of k movies
 * @param theKMap the vector of k movies
 * @return the movie with the minimum rank
 */
    int _readMoviesAtt(const string& moviesAttributesFilePath);
    /**
     * @brief reads the user ratings from the file
     * @param userRanksFilePath the filepath
     */
    int _readUserRanks(const string& userRanksFilePath);

    /**
     * @brief number of movies to rate;
     */
    int _numberOfMovies;
    /**
     * @brief number of customers
     */
    int _numberOfCustomers;
    /**
     * @brief number of categories that movies get evaluated by
     */
    int _numberOfEvaluations;

    /**
      * @brief gets the norm of a double vector
     * @param vec the vector
     * @return the norm
     */
    double _normFinder(const vector<double> vec) const;

    /**
     * @brief multiplies an int with a double vector
     * @param a the first vector
     * @param b the second vector
     * @return the multiplication
     */
    double _multiply(const vector<double> a, const vector<double> b) const;
};

#endif //EX5_RECOMMENDERSYSTEM_H
