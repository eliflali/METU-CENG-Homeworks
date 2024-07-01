:- module(main, [is_movie_directed_by/2, total_awards_nominated/2, all_movies_directed_by/2, total_movies_released_in/3, all_movies_released_between/4]).
:- [kb].

% DO NOT CHANGE THE UPPER CONTENT, WRITE YOUR CODE AFTER THIS LINE
 %is_movie_directed_by(title, director)  
 %movie(Name, Director, Year, OscarNoms, EmmyNoms, GoldenGlobeNoms).
 %total_awards_nominated ( Title , Nominations ) .
is_movie_directed_by(X, Y) :- movie(X, Y, _,_,_,_).

total_awards_nominated(Title, Nominations) :- 
                movie(Title, _,_,Oscar, Emmy, GG), 
                Nominations is Oscar + Emmy + GG.


%findall(X,descend(martha,X),Z).


all_movies_directed_by(Director, Movies) :- 
                findall(Title, (movie(Title, Director, _,_,_,_)), Movies).

helper([],Year,Result, Count).

helper([Movie|Rest], Year, Result+1, Count) :-
        movie(Movie,_,Yearo,_,_,_),  
        Yearo =:= Year,
        helper(Rest, Year, Result+1, Count). 
        
helper([Movie|Rest], Year, Result, Count) :-
        movie(Movie,_,Yearo,_,_,_),  
        Yearo =\= Year, 
        helper(Rest, Year, Result, Count). 


total_movies_released_in(Movies, Year, Count) :-
                %movie(Movie,_,Yearo,_,_,_),  
                helper(Movies, Year, 0, Count).


/*helper([],Year,Count, Count).
helper([Movie|Rest], Year, Count, Result) :-
        movie(Movie,_,Yearo,_,_,_),  
        X is Count+(Yearo =:= Year), helper(Rest, Year, X, Result).*/
                
all_movies_released_between([],_,_,[]).
all_movies_released_between([Movie|Rest] , MinYear , MaxYear , [Movie|MoviesBetweenGivenYears]) :-
                movie(Movie,_,Year,_,_,_),
                Year =< MaxYear, Year >=MinYear,
                all_movies_released_between(Rest, MinYear, MaxYear, MoviesBetweenGivenYears).
all_movies_released_between([Movie|Rest],MinYear , MaxYear , MoviesBetweenGivenYears) :-
                movie(Movie,_,Year,_,_,_),
                Year < MinYear,
                all_movies_released_between(Rest, MinYear, MaxYear, MoviesBetweenGivenYears).
all_movies_released_between([Movie|Rest], MinYear , MaxYear , MoviesBetweenGivenYears) :-
                movie(Movie,_,Year,_,_,_),
                Year > MaxYear,
                all_movies_released_between(Rest, MinYear, MaxYear, MoviesBetweenGivenYears). 
