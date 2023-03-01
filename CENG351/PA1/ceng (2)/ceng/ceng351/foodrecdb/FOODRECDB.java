package ceng.ceng351.foodrecdb;

import java.sql.*;
import java.util.ArrayList;
import java.util.Vector;

public class FOODRECDB implements ceng.ceng351.foodrecdb.IFOODRECDB {

    private static String user = "e2448694"; // TODO: Your userName
    private static String password = "9I7x9UNGe5qaXPNo"; //  TODO: Your password
    private static String host = "momcorp.ceng.metu.edu.tr"; // host name
    private static String database = "db2448694"; // TODO: Your database name
    private static int port = 8080; // port
    private static Connection connection = null;

    @Override
    public void initialize() {
        String url = "jdbc:mysql://" + host + ":" + port + "/" + database + "?useSSL=false";;

        try {
            Class.forName("com.mysql.cj.jdbc.Driver");
            connection =  DriverManager.getConnection(url, user, password);
        }
        catch (SQLException | ClassNotFoundException e) {
            e.printStackTrace();
        }
    }

    @Override
    public int createTables() {
        int numberOfTablesCreated = 0;
        Vector<String> queries = new Vector<>();
        String queryCreateMenuItemsTable = "Create Table if not exists MenuItems (" +
                "itemID int not null," +
                "itemName varchar(40)," +
                "cuisine varchar(20)," +
                "price int," +
                "primary key (itemID))";

        queries.add(queryCreateMenuItemsTable);

        String queryCreateIngredientsTable = "Create Table if not exists Ingredients (" +
                "ingredientID int not null," +
                "ingredientName varchar(40)," +
                "primary key (ingredientID))";

        queries.add(queryCreateIngredientsTable);


        String queryCreateIncludesTable = "Create Table if not exists Includes (" +
                "itemID int not null," +
                "ingredientID int not null," +
                "primary key (itemID, ingredientID),"+
                "foreign key (itemID) references MenuItems(itemID),"+
                "foreign key (ingredientID) references Ingredients(ingredientID)"+
                "on update cascade)";

        queries.add(queryCreateIncludesTable);


        String queryCreateRatingsTable = "Create Table if not exists Ratings (" +
                "ratingID int not null," +
                "itemID int," +
                "rating int," +
                "ratingDate date," +
                "primary key (ratingID),"+
                "foreign key (itemID) references MenuItems(itemID)" +
                "on update cascade)";

        queries.add(queryCreateRatingsTable);

        String queryCreateDietaryCategoriesTable = "Create Table if not exists DietaryCategories (" +
                "ingredientID int not null," +
                "dietaryCategory varchar(20)," +
                "primary key (ingredientID, dietaryCategory),"+
                "foreign key (ingredientID) references Ingredients(ingredientID)" +
                "on delete cascade)" ;

        queries.add(queryCreateDietaryCategoriesTable);

        for (int i = 0; i < queries.size(); i++)
        {
            try {

                Statement statement = connection.createStatement();
                statement.executeUpdate(queries.get(i));
                numberOfTablesCreated++;
                statement.close();
            }
            catch (SQLException e) {
                e.printStackTrace();
            }

        }
        return numberOfTablesCreated;
    }

    @Override
    public int dropTables() {
        int numberOfTablesDropped = 0;
        Vector<String> queriesDrop = new Vector<>();


        String queryDropIncludesTable = "DROP TABLE if exists Includes";
        queriesDrop.add(queryDropIncludesTable);
        String queryDropRatingsTable = "DROP TABLE if exists Ratings";
        queriesDrop.add(queryDropRatingsTable);
        String queryDropDietaryCategoriesTable = "DROP TABLE if exists DietaryCategories";
        queriesDrop.add(queryDropDietaryCategoriesTable);
        String queryDropMenuItemsTable = "DROP TABLE if exists MenuItems";
        queriesDrop.add(queryDropMenuItemsTable);
        String queryDropIngredientsTable = "DROP TABLE if exists Ingredients";
        queriesDrop.add(queryDropIngredientsTable);



        for (int i = 0; i < queriesDrop.size(); i++)
        {
            try {

                Statement statement = connection.createStatement();
                statement.executeUpdate(queriesDrop.get(i));
                numberOfTablesDropped++;
                statement.close();
            }
            catch (SQLException e) {
                e.printStackTrace();
            }

        }

        return numberOfTablesDropped;
    }

    @Override
    public int insertMenuItems(ceng.ceng351.foodrecdb.MenuItem[] items) {
        int numberOfMICreated = 0;

        for(int i = 0; i<items.length; i++)
        {
            String query = "INSERT INTO MenuItems(itemID, itemName, cuisine, price) values (?, ?, ?, ?) ";
            try
            {
                PreparedStatement ready = connection.prepareStatement(query);

                ready.setInt(1, items[i].getItemID());
                ready.setString(2, items[i].getItemName());
                ready.setString(3, items[i].getCuisine());
                ready.setInt(4, items[i].getPrice());

                ready.execute();
                numberOfMICreated++;
            }
            catch(SQLException e)
            {
                e.printStackTrace();
            }
        }

        return numberOfMICreated;
    }

    @Override
    public int insertIngredients(Ingredient[] ingredients) {
        int numberOfIngCreated = 0;

        for(int i = 0; i<ingredients.length; i++)
        {
            String query = "INSERT INTO Ingredients(ingredientID, ingredientName) values (?, ?) ";
            try
            {
                PreparedStatement ready = connection.prepareStatement(query);

                ready.setInt(1, ingredients[i].getIngredientID());
                ready.setString(2, ingredients[i].getIngredientName());

                ready.execute();
                numberOfIngCreated++;
            }
            catch(SQLException e)
            {
                e.printStackTrace();
            }
        }

        return numberOfIngCreated;
    }
    @Override
    public int insertRatings(Rating[] ratings) {
        int numberOfRatCreated = 0;

        for(int i = 0; i<ratings.length; i++)
        {
            String query = "INSERT INTO Ratings(ratingID, itemID, rating, ratingDate) values (?, ?, ?, ?) ";
            try
            {
                PreparedStatement ready = connection.prepareStatement(query);

                ready.setInt(1, ratings[i].getRatingID());
                ready.setInt(2, ratings[i].getItemID());
                ready.setInt(3, ratings[i].getRating());
                ready.setString(4, ratings[i].getRatingDate());

                ready.execute();
                numberOfRatCreated++;
            }
            catch(SQLException e)
            {
                e.printStackTrace();
            }
        }

        return numberOfRatCreated;
    }

    @Override
    public int insertDietaryCategories(DietaryCategory[] categories) {
        int numberOfDCCreated = 0;

        for(int i = 0; i<categories.length; i++)
        {
            String query = "INSERT INTO DietaryCategories(ingredientID, dietaryCategory) values (?, ?) ";
            try
            {
                PreparedStatement ready = connection.prepareStatement(query);

                ready.setInt(1, categories[i].getIngredientID());
                ready.setString(2, categories[i].getDietaryCategory());

                ready.execute();
                numberOfDCCreated++;
            }
            catch(SQLException e)
            {
                e.printStackTrace();
            }
        }

        return numberOfDCCreated;
    }



    @Override
    public int insertIncludes(Includes[] includes) {
        int numberOfInclCreated = 0;

        for(int i = 0; i<includes.length; i++)
        {
            String query = "INSERT INTO Includes(itemID, ingredientID) values (?, ?) ";
            try
            {
                PreparedStatement ready = connection.prepareStatement(query);

                ready.setInt(1, includes[i].getItemID());
                ready.setInt(2, includes[i].getIngredientID());

                ready.execute();
                numberOfInclCreated++;
            }
            catch(SQLException e)
            {
                e.printStackTrace();
            }
        }

        return numberOfInclCreated;
    }

    @Override
    public MenuItem[] getMenuItemsWithGivenIngredient(String name) {
        ArrayList<MenuItem> result = new ArrayList<>();
        MenuItem Item;
        MenuItem[] Items;
        ResultSet queryResult;

        String q = "SELECT M.itemID, M.itemName, M.cuisine, M.price "+
                "FROM MenuItems M " +
                "WHERE M.itemID IN ("+
                                "SELECT N.itemID "+
                                "FROM Ingredients I, Includes N "+
                                "WHERE I.ingredientName = '%s' AND I.ingredientID = N.ingredientID)"+
                "ORDER BY M.itemID asc";

        String query = String.format(q, name);


        try{
            Statement statement = connection.createStatement();
            queryResult = statement.executeQuery(query);

            while(queryResult.next())
            {
                int itemID = queryResult.getInt("itemID");
                String itemName = queryResult.getString("itemName");
                String cuisine = queryResult.getString("cuisine");
                int price = queryResult.getInt("price");

                Item = new MenuItem(itemID, itemName, cuisine, price);

                result.add(Item);

            }
        }
        catch(SQLException e)
        {
            e.printStackTrace();
        }



        Items = new MenuItem[result.size()];
        Items = result.toArray(Items);

        return result.toArray(new MenuItem[result.size()]);
    }

    @Override
    public MenuItem[] getMenuItemsWithoutAnyIngredient() {
        ArrayList<MenuItem> result = new ArrayList<>();
        MenuItem Item;
        MenuItem[] Items;
        ResultSet queryResult;

        String query = "SELECT M.itemID, M.itemName, M.cuisine, M.price " +
                "FROM MenuItems M " +
                "WHERE M.itemID NOT IN (" +
                "SELECT N.itemID " +
                "FROM Ingredients I, Includes N " +
                "WHERE I.ingredientID = N.ingredientID)"+
                "ORDER BY M.itemID asc";


        try {
            Statement statement = connection.createStatement();
            queryResult = statement.executeQuery(query);

            while (queryResult.next()) {
                int itemID = queryResult.getInt("itemID");
                String itemName = queryResult.getString("itemName");
                String cuisine = queryResult.getString("cuisine");
                int price = queryResult.getInt("price");

                Item = new MenuItem(itemID, itemName, cuisine, price);

                result.add(Item);

            }
        } catch (SQLException e) {
            e.printStackTrace();
        }


        Items = new MenuItem[result.size()];
        Items = result.toArray(Items);

        return result.toArray(new MenuItem[result.size()]);
    }

    @Override
    public Ingredient[] getNotIncludedIngredients() {
        ArrayList<Ingredient> result = new ArrayList<>();
        Ingredient ingredient;
        Ingredient[] ingredients;
        ResultSet queryResult;

        String query = "SELECT I.ingredientID, I.ingredientName "+
                "FROM Ingredients I " +
                "WHERE I.ingredientID NOT IN ("+
                "SELECT N.ingredientID "+
                "FROM Includes N "+
                "WHERE I.ingredientID = N.ingredientID)"+
                "ORDER BY I.ingredientID asc";


        try{
            Statement statement = connection.createStatement();
            queryResult = statement.executeQuery(query);

            while(queryResult.next())
            {
                int ingredientID = queryResult.getInt("ingredientID");
                String ingredientName = queryResult.getString("ingredientName");

                 ingredient = new Ingredient(ingredientID, ingredientName);

                result.add(ingredient);

            }
        }
        catch(SQLException e)
        {
            e.printStackTrace();
        }



        ingredients = new Ingredient[result.size()];
        ingredients = result.toArray(ingredients);

        return result.toArray(new Ingredient[result.size()]);
    }

    @Override
    public MenuItem getMenuItemWithMostIngredients() {
        ArrayList<MenuItem> result = new ArrayList<>();
        MenuItem Item = null;
        ResultSet queryResult;

        String query = "SELECT * "+
                "FROM MenuItems M "+
                "WHERE M.itemID IN ("+
                                "SELECT N.itemID "+
                                "FROM Includes N "+
                                "GROUP BY N.itemID "+
                                "HAVING COUNT(*) = (SELECT MAX(counted.counts) " +
                                                    "FROM (SELECT COUNT(*) AS counts "+
                                                            "FROM Includes N2 "+
                                                            "GROUP BY N2.itemID)as counted))";


        try{
            Statement statement = connection.createStatement();
            queryResult = statement.executeQuery(query);

            while(queryResult.next())
            {
                int itemID = queryResult.getInt("itemID");
                String itemName = queryResult.getString("itemName");
                String cuisine = queryResult.getString("cuisine");
                int price = queryResult.getInt("price");

                Item = new MenuItem(itemID, itemName, cuisine, price);


            }
        }
        catch(SQLException e)
        {
            e.printStackTrace();
        }



        return Item;
    }

    @Override
    public QueryResult.MenuItemAverageRatingResult[] getMenuItemsWithAvgRatings()
    {

        ArrayList<QueryResult.MenuItemAverageRatingResult> result = new ArrayList<>();
        QueryResult.MenuItemAverageRatingResult Item;
        QueryResult.MenuItemAverageRatingResult[] Items;
        ResultSet queryResult;

        String query = "(SELECT M.itemID, M.itemName, AVG(R.rating) AS avgRating "+
                "FROM MenuItems M, Ratings R "+
                "WHERE M.itemID = R.itemID " +
                "GROUP BY M.itemID)"+
                "UNION "+
                "(SELECT M.itemID, M.itemName, NULL "+
                "FROM MenuItems M "+
                "WHERE M.itemID NOT IN (SELECT R.itemID "+
                                        "FROM Ratings R)) "+
                "ORDER BY avgRating desc";



        try{
            Statement statement = connection.createStatement();
            queryResult = statement.executeQuery(query);

            while(queryResult.next())
            {
                String itemID = queryResult.getString("itemID");
                String itemName = queryResult.getString("itemName");
                String avgRating = queryResult.getString("avgRating");

                Item = new QueryResult.MenuItemAverageRatingResult(itemID, itemName, avgRating);

                result.add(Item);

            }
        }
        catch(SQLException e)
        {
            e.printStackTrace();
        }



        Items = new QueryResult.MenuItemAverageRatingResult[result.size()];
        Items = result.toArray(Items);

        return result.toArray(new QueryResult.MenuItemAverageRatingResult[result.size()]);
    }

    @Override
    public MenuItem[] getMenuItemsForDietaryCategory(String category) {
        ArrayList<MenuItem> result = new ArrayList<>();
        MenuItem Item;
        MenuItem[] Items;
        ResultSet queryResult;

        String q = "SELECT DISTINCT M.itemID, M.itemName, M.cuisine, M.price "+
                "FROM MenuItems M " +
                "WHERE M.itemID NOT IN (SELECT N.itemID "+
                                        "FROM Includes N, (SELECT D.ingredientID as ingredient " +
                "                                           FROM DietaryCategories D " +
                "                                           WHERE NOT EXISTS (SELECT D1.ingredientID " +
                "                                                           FROM DietaryCategories D1 " +
                "                                                           WHERE D1.dietaryCategory = '%s' AND D.ingredientID = D1.ingredientID)) as others "+
                                        "WHERE N.ingredientID = others.ingredient) " +
                        "AND M.itemID IN (SELECT A.itemID " +
                                          "FROM Includes A) "+
                "ORDER BY M.itemID asc";

        String query = String.format(q, category);


        try{
            Statement statement = connection.createStatement();
            queryResult = statement.executeQuery(query);

            while(queryResult.next())
            {
                int itemID = queryResult.getInt("itemID");
                String itemName = queryResult.getString("itemName");
                String cuisine = queryResult.getString("cuisine");
                int price = queryResult.getInt("price");

                Item = new MenuItem(itemID, itemName, cuisine, price);

                result.add(Item);

            }
        }
        catch(SQLException e)
        {
            e.printStackTrace();
        }



        Items = new MenuItem[result.size()];
        Items = result.toArray(Items);

        return result.toArray(new MenuItem[result.size()]);
    }

    @Override
    public Ingredient getMostUsedIngredient() {
        ArrayList<Ingredient> result = new ArrayList<>();
        Ingredient ingredient = null;
        ResultSet queryResult;

        String query = "SELECT * "+
                        "FROM Ingredients I "+
                        "WHERE I.ingredientID IN ("+
                                            "SELECT DISTINCT N.ingredientID "+
                                            "FROM Includes N "+
                                            "GROUP BY N.ingredientID "+
                                            "HAVING COUNT(*) = (SELECT MAX(counted.counts) " +
                                                                "FROM (SELECT COUNT(*) AS counts "+
                                                                        "FROM Includes N2 "+
                                                                        "GROUP BY N2.ingredientID) as counted))";



        try{
            Statement statement = connection.createStatement();
            queryResult = statement.executeQuery(query);

            while(queryResult.next())
            {
                int ingredientID = queryResult.getInt("ingredientID");
                String ingredientName = queryResult.getString("ingredientName");

                ingredient = new Ingredient(ingredientID, ingredientName);


            }
        }
        catch(SQLException e)
        {
            e.printStackTrace();
        }



        return ingredient;
    }

    @Override
    public QueryResult.CuisineWithAverageResult[] getCuisinesWithAvgRating() {
        ArrayList<QueryResult.CuisineWithAverageResult> result = new ArrayList<>();
        QueryResult.CuisineWithAverageResult Item;
        QueryResult.CuisineWithAverageResult[] Items;
        ResultSet queryResult;

        String query = "(SELECT M.cuisine, AVG(rat.rate) AS averageRating "+
                        "FROM MenuItems M, (SELECT R.rating AS rate, R.itemID as item " +
                                            "FROM Ratings R " +
                                            "WHERE R.rating <> 0) AS rat "+
                        "WHERE M.itemID = rat.item " +
                        "GROUP BY M.cuisine) " +
                        "UNION "+
                        "(SELECT M.cuisine, NULL "+
                        "FROM MenuItems M "+
                        "WHERE M.cuisine NOT IN (SELECT M2.cuisine "+
                                                "FROM Ratings R, MenuItems M2 " +
                                                "WHERE R.itemID = M2.itemID)) "+
                        "ORDER BY averageRating desc";



        try{
            Statement statement = connection.createStatement();
            queryResult = statement.executeQuery(query);

            while(queryResult.next())
            {
                String cuisineName = queryResult.getString("cuisine");
                String averageRating = queryResult.getString("averageRating");

                Item = new QueryResult.CuisineWithAverageResult(cuisineName, averageRating);

                result.add(Item);

            }
        }
        catch(SQLException e)
        {
            e.printStackTrace();
        }



        Items = new QueryResult.CuisineWithAverageResult[result.size()];
        Items = result.toArray(Items);

        return result.toArray(new QueryResult.CuisineWithAverageResult[result.size()]);
    }

    @Override
    public QueryResult.CuisineWithAverageResult[] getCuisinesWithAvgIngredientCount() {
        ArrayList<QueryResult.CuisineWithAverageResult> result = new ArrayList<>();
        QueryResult.CuisineWithAverageResult Item;
        QueryResult.CuisineWithAverageResult[] Items;
        ResultSet queryResult;

        String query = "(SELECT M.cuisine, AVG(temp.count) AS average "+
                "FROM MenuItems M, ((SELECT M5.itemID as item, COUNT(*) as count " +
                                    "FROM MenuItems M5, Includes I " +
                                    "WHERE M5.itemID = I.itemID " +
                                    "GROUP BY M5.itemID) " +
                                    "UNION " +
                                    "(SELECT mi.itemID as item, 0 as COUNT " +
                                    "FROM MenuItems mi " +
                                    "WHERE mi.itemID NOT IN (SELECT N.itemID " +
                                                    "FROM Includes N))) as temp "+
                "WHERE M.itemID = temp.item " +
                "GROUP BY M.cuisine) " +
                "UNION "+
                "(SELECT M.cuisine, 0 "+
                "FROM MenuItems M "+
                "WHERE M.cuisine NOT IN (SELECT M2.cuisine "+
                "FROM Ratings R, MenuItems M2 " +
                "WHERE R.itemID = M2.itemID)) "+
                "ORDER BY average desc";

        try{
            Statement statement = connection.createStatement();
            queryResult = statement.executeQuery(query);

            while(queryResult.next())
            {
                String cuisineName = queryResult.getString("cuisine");
                String average = queryResult.getString("average");

                Item = new QueryResult.CuisineWithAverageResult(cuisineName, average);

                result.add(Item);

            }
        }
        catch(SQLException e)
        {
            e.printStackTrace();
        }



        Items = new QueryResult.CuisineWithAverageResult[result.size()];
        Items = result.toArray(Items);

        return result.toArray(new QueryResult.CuisineWithAverageResult[result.size()]);
    }

    @Override
    public int increasePrice(String ingredientName, String increaseAmount)
    {
        int numberOfRowsAffected = 0;
        String q ="UPDATE MenuItems " +
                "SET MenuItems.price = MenuItems.price+ '%s' "+
                "WHERE MenuItems.itemID IN (SELECT N.itemID " +
                                        "FROM Includes N, Ingredients I " +
                                        "WHERE N.ingredientID = I.ingredientID AND I.ingredientName = '%s')";

        String query = String.format(q, increaseAmount, ingredientName);


        try {
            Statement statement = connection.createStatement();
            numberOfRowsAffected = statement.executeUpdate(query);
            statement.close();
        } catch (SQLException e) {
            e.printStackTrace();
        }

        return numberOfRowsAffected;
    }

    @Override
    public Rating[] deleteOlderRatings(String date)
    {
        ArrayList<Rating> result = new ArrayList<>();
        Rating ratin;
        Rating[] ratins;
        ResultSet queryResult;
        String query2 ="SELECT R.ratingID,R.itemID,R.rating,R.ratingDate " +
                "FROM Ratings R " +
                "WHERE R.ratingDate<'" + date +"'" +
                "ORDER BY R.ratingID asc";


        String query="DELETE " +
                "FROM Ratings  " +
                "WHERE Ratings.ratingDate < '" + date +"'" + ';';

        try{
            Statement statement = connection.createStatement();
            queryResult = statement.executeQuery(query2);

            while(queryResult.next())
            {
                int ratingID = queryResult.getInt("ratingID");
                int itemID = queryResult.getInt("itemID");
                int rating = queryResult.getInt("rating");
                String ratingDate = queryResult.getString("ratingDate");

                ratin = new Rating(ratingID, itemID, rating, ratingDate);

                result.add(ratin);
            }
        }
        catch(SQLException e)
        {
            e.printStackTrace();
        }

        try {
            Statement statement = connection.createStatement();
            statement.executeUpdate(query);

            statement.close();
        }
        catch (SQLException e) {
            e.printStackTrace();
        }
        ratins = new Rating[result.size()];
        ratins = result.toArray(ratins);

        return result.toArray(new Rating[result.size()]);
    }


}
