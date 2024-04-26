A. General
This project is about analyzing the demand for films based on category.
The main business questions addressed are:

1. Which categories generate the most demand?
2. How many films are there for each category?
3. Does the inventory of dvds for each category match this demand? or are there too few titles of popular categories and too many titles within less popular categories?

The DVD business will benefit from this report by being presented with a summary report that calculates any discrepancy between inventory levels and demand for each category, which serve to suggest which film categories to increase purchases of and which to purchase fewer of. By adding inventory of films from popular categories and lowering inventory levels of less popular film categories, the company can increase profits by boosting revenues from popular film categories or lowering expenditures associated with storing dvds of less popular film categories.

A.1 Specific fields included in the detailed and summary tables

<!-- detailed table -->

The detailed table contains the following four fields from four joined tables (`category`, `film`, `inventory`, `rental`):

- `category.name`
- `film.film_id`
- `inventory.inventory_id`
- `rental.rental_id`

<!-- summary table -->

The summary table consists of the following six fields:

- `category_name`
- `films_in_category`
- `rentals_per_category`
- `rentals_per_film`
- `inventory_rank`
- `rank_rentals_per_film`
- `demand_inventory_discrepancy`

A.2 Types of data fields used for the report

The detailed table contains of varchar and integer data types distributed among its four fields in the following manner:

- `category.name` (varchar: a 25 character varchar film category of the dvd)
- `film.film_id` (integer: the join predicate between the film and inventory tables)
- `inventory.inventory_id` (integer: the join predicate between the inventory and rental tables)
- `rental.rental_id` (integer: the primary key of the rental table)

The summary table contains varchar, bigint and numeric data types distributed in the following manner:

- `category_name` (varchar: a 25 character varchar film category of the dvd and the GROUP BY key for the table)
- `films_in_category` (bigint: a COUNT aggregation of distinct film_ids in order to arrive at the number of films for the category)
- `rentals_per_category` (bigint: a COUNT aggregation of rental_ids in order to arrive at the total number of rentals for this category)
- `rentals_per_film` (numeric: a rounded division of rentals_per category divided by films_in_category)
- `inventory_rank` (bigint: a rank number for each row via the RANK() function, with the 1st rank being the category with the largest number inventory)
- `rank_rentals_per_film` (bigint: A rank of each film category based on their value on the preceding column rentals_per_film)
- `demand_inventory_discrepancy` (bigint: a calculated numeric value of the difference between the rank_rentals_per_film and inventory_rank columns)

A.3 The five dataset tables used for the detailed and summary tables

The `category`, `film_category`, `film`, `inventory`, and `rental` dataset tables are used in this report for the detailed and summary tables.

A.4: Field in the detailed table requiring custom transformation with user-defined function

I created a user-defined function called `remove_new_category()` that transforms the film_category varchar value from 'New' to 'Unknown' in the detailed table. The reason is that 'New' is not a category name or film genre, but merely a temporary status that refers to the age of the film. I therefore created a user-defined function that transforms that value to 'Unknown', which signifies that this particular title could be any of film category names and that the value should be updated once the category of the film is known.

A.5. Business uses of the detailed and summary tables

The detailed table is a generated from a join query of the four underlying tables. This table gathers all the raw data used for the summary table, and in this way it performs the role of a data mart for the analytics of this report. It is therefore necessary that this table is never stale when used to generate the summary table, so this table will be updated before the summary table by the trigger that updates both.

The summary table provides all the columns and values of interest to the end user, which is imagined to be the person responsible for purchases of new dvds, or of sales of dvds with little demand.
This person will thus always have an overview of demand for films in each category, any discrepancy between the demand and inventory (for example if the most in-demand film category is far down the list in inventory rank). The `demand_inventory_discrepancy` column for each film_category is the metric on which to base film purchases on. If this value is negative, then there are too few titles of this category and vice versa.

A6. How frequently the report should be refreshed

The detailed and summary tables should be refreshed every 24 hours after midnight. The advantage of this is that the query would take place when system traffic is at its lowest (assuming that users are not evenly distributed across all global time zones). A further advantage of a nightly update is that the previous day's activity will be included in the update, which generates a fresh table ready for the end user at the beginning of each business day. If purchases are not made during the weekend, then then this job could be omitted Friday and Saturday night so that the Sunday night job creates a report for use Monday morning.

The reason the cron job should run daily instead of weekly is that the person in charge of sales can be expected to make dvd purchase and/or sales decisions every day or on any given day, which means that this person will be able to make informed decisions based on the most recent data. An added advantage of daily refresh rates is that any purchases or sales the day before will be included in the updated data for the following day, thus avoiding duplicate purchases for a single category due to past previous purchases not being accounted for. In the case where purchases decisions were to take place multiple times in a day and where previous purchases that day affects later purchases, then the script called by the cron job could be executed manually at decision time in order to generate an updated table.

B. Original code for function(s) from A.4 perform transformations.

```sql
CREATE OR REPLACE FUNCTION remove_new_category(category_name TEXT)
RETURNS TEXT AS $$

BEGIN
  IF category_name = 'New' THEN
    RETURN 'Unknown';
  ELSE
    RETURN category_name;
  END IF;
END;

$$ language plpgsql;
```

C. Original SQL code of detailed and summary tables

<!-- Detailed table: -->

```sql
SELECT remove_new_category(category.name) AS category_name,
       film.film_id AS film_id,
       inventory.inventory_id AS inventory_id,
       rental.rental_id AS rental_id
INTO detailed_table
FROM category
INNER JOIN film_category
  ON category.category_id = film_category.category_id
INNER JOIN film
  ON film.film_id = film_category.film_id
INNER JOIN inventory
  ON film.film_id = inventory.film_id
INNER JOIN rental
  ON inventory.inventory_id = rental.inventory_id;
```

<!-- Summary table: -->

```sql
SELECT category_name,
      COUNT(DISTINCT film_id) AS films_in_category,
      COUNT(rental_id) AS rentals_per_category,
      ROUND(COUNT(rental_id) / COUNT(DISTINCT film_id)::Numeric, 2 ) AS rentals_per_film,
      RANK() OVER (ORDER BY COUNT(DISTINCT film_id) DESC) AS inventory_rank,
      RANK() OVER(ORDER BY (COUNT(rental_id) / COUNT(DISTINCT film_id)::Numeric) DESC) AS rank_rentals_per_film,
      RANK() OVER(ORDER BY (COUNT(rental_id) / COUNT(DISTINCT film_id)::Numeric) DESC) - RANK() OVER (ORDER BY COUNT(DISTINCT film_id) DESC) AS inventory_demand_discrepancy
INTO summary_table
FROM detailed_table
GROUP BY category_name
ORDER BY inventory_demand_discrepancy;
```

D. Extraction of raw data needed for the detailed sections.

<!-- Detailed table: -->

```sql
SELECT category.name AS category_name,
       film.film_id AS film_id,
       inventory.inventory_id AS inventory_id,
       rental.rental_id AS rental_id
INTO detailed_table;
FROM category
INNER JOIN film_category
  ON category.category_id = film_category.category_id
INNER JOIN film
  ON film.film_id = film_category.film_id
INNER JOIN inventory
  ON film.film_id = inventory.film_id
INNER JOIN rental
  ON inventory.inventory_id = rental.inventory_id;
```

E. Trigger that updates the summary table as data is added to, deleted from or updated on the detailed table.

```sql
CREATE TRIGGER detailed_table_update_trigger
     AFTER INSERT OR DELETE OR UPDATE
     ON detailed_table
     EXECUTE FUNCTION update_summary_table();
```

```sql
CREATE OR REPLACE FUNCTION update_summary_table()
RETURNS TRIGGER AS
$$
BEGIN
  DELETE FROM summary_table;

  INSERT INTO summary_table(category_name, films_in_category, rentals_per_category, rentals_per_film, inventory_rank, rank_rentals_per_film, inventory_demand_discrepancy)
  SELECT category_name,
         COUNT(DISTINCT film_id) AS films_in_category,
         COUNT(rental_id) AS rentals_per_category,
         ROUND(COUNT(rental_id) / COUNT(DISTINCT film_id)::Numeric, 2) AS rentals_per_film,
         RANK() OVER (ORDER BY COUNT(DISTINCT film_id) DESC) AS inventory_rank,
         RANK() OVER(ORDER BY (COUNT(rental_id) / COUNT(DISTINCT film_id)::Numeric) DESC) AS
           rank_rentals_per_film,
         RANK() OVER(ORDER BY (COUNT(rental_id) / COUNT(DISTINCT film_id)::Numeric) DESC) - RANK()
           OVER (ORDER BY COUNT(DISTINCT film_id) DESC) AS inventory_demand_discrepancy
FROM detailed_table
GROUP BY category_name
ORDER BY inventory_demand_discrepancy;

RETURN NEW;
END;
$$
language plpgsql;
```

F. Original stored procedure to refresh the detailed and summary tables.
NB: Because there's already a trigger on the detailed table that updates the summary table, this procedure only needs to refresh the detailed table for both tables to be updated

```sql
CREATE PROCEDURE update_detailed_and_summary_tables()
LANGUAGE SQL
AS $$

DELETE FROM detailed_table;

INSERT INTO detailed_table(category_name, film_id, inventory_id, rental_id)
SELECT remove_new_category(category.name) AS category_name,
       film.film_id AS film_id,
       inventory.inventory_id AS inventory_id,
       rental.rental_id AS rental_id
FROM category
INNER JOIN film_category
  ON category.category_id = film_category.category_id
INNER JOIN film
  ON film.film_id = film_category.film_id
INNER JOIN inventory
  ON film.film_id = inventory.film_id
INNER JOIN rental
  ON inventory.inventory_id = rental.inventory_id;
$$;
```

F.1 job scheduling tool
I would implement a daily linux cron job to run a sql script that calls this procedure, `update_detailed_and_summary_tables`, which tears down and recreates the tables very 24 hours post midnight. This is straight forward to implement, the timing can easily be altered, and no further extensions are needed since cron daemons are native to the operating system.

H. No third-party code or references were made in this report.
