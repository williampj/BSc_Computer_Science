A. General
This project is about analyzing the demand for films based on category.
The main business questions addressed are

1. Which categories generate the most demand?
2. How many films are there for each category?
3. Does the inventory of dvds for each category match this demand? or are there too few titles of popular categories and too many titles within less popular categories?

The DVD business will benefit from this report by being presented with a summary report that calculates any discrepancy between inventory levels and demand for each category along with recommendations of which film categories to increase purchases of and which to purchase fewer of. By adding inventory of films from popular categories and lowering inventory levels of less popular film categories, the company can increase profits by boosting revenues from popular film categories or lowering expenditures associated with storing dvds of less popular film categories.

A.1-A.3 This business report will contain 4 detailed view tables and a summary table.
NB The code for all of the tables are in Section C.

1ST DETAILED TABLE:
The first detailed table, `category_film_inventory_rental_view` will be a materialized view table that joins four underlying tables, without aggregated columns. This table will refresh via trigger functions whenever there is a relevant deletion, insertion or udpate of its joined tables.
The table consists of the following four fields from four joined tables (`category`, `film`, `inventory`, `rental`):

- `category.name` (a varchar film category of the dvd)
- `film.film_id` (a numeric join predicate between the film and inventory tables)
- `inventory.inventory_id` (a numeric join predicate between the inventory and rental tables)
- `rental.rental_id` (the numeric primary key of the rental table)

```sql
CREATE MATERIALIZED VIEW IF NOT EXISTS category_film_inventory_rental_view AS
SELECT category.name AS category_name,
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
```

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

2ND DETAILED TABLE:

The second table, `view_ordered_by_rentals_per_category`, is a non-materialized view table that is derived from the previous materialized table `view_ordered_by_rentals_per_category` in the FROM clause. It consists of the following three fields:

- `category_name` (varchar film category of the dvd and the GROUP BY key for this table)
- `films_in_category` (a numeric COUNT aggregation of distinct film_ids in order to arrive at the number of films for the category)
- `rentals_per_category` (a COUNT aggregation of rental_ids in order to arrive at the total number of rentals for this category)

```sql
CREATE OR REPLACE VIEW view_ordered_by_rentals_per_category AS
SELECT category_name,
      COUNT(DISTINCT film_id) AS films_in_category,
      COUNT(rental_id) AS rentals_per_category
FROM category_film_inventory_rental_view
GROUP BY category_name
ORDER BY rentals_per_category DESC;
```

3RD DETAILED TABLE:

The third table, `view_ordered_by_films_in_category`, is a non-materialized view table that is likewise derived from the first table, `view_ordered_by_rentals_per_category`. It consistes of the following three fields:

- `category_name` (film category of the dvd and the GROUP BY key for this table)
- `films_in_category` (a numeric COUNT aggregation of distinct film_ids in order to arrive at the number of films for the category)
- `inventory_rank` (a rank number for each row via the RANK() function, with the 1st rank being the category with the largest number inventory)

```sql
CREATE OR REPLACE VIEW view_ordered_by_films_in_category AS
SELECT category_name,
      COUNT(DISTINCT film_id) AS films_in_category,
      RANK() OVER (ORDER BY COUNT(DISTINCT film_id) DESC) AS inventory_rank
FROM category_film_inventory_rental_view
GROUP BY category_name
ORDER BY films_in_category DESC;
```

4TH DETAILED TABLE:

The fourth and final detailed table is the view table `view_ordered_by_rentals_per_film`. It is derived from the `view_ordered_by_rentals_per_category` view table and consists of the following three fiedls:

- `category_name` (category for the dvd title)
- `rentals_per_film` (a rounded division of rentals_per category divided by films_in_category. The rentals_per_category value is transformed by casting it to float beforehand in order to avoid integer division which cuts off decimal values by flooring the quotient)
- `rank_rentals_per_film` (A rank of each film category based on their value on the preceding column rentals_per_film)

```sql
CREATE OR REPLACE VIEW view_ordered_by_rentals_per_film AS
SELECT category_name,
      ROUND(rentals_per_category / films_in_category::Numeric, 2) AS rentals_per_film,
      RANK() OVER(ORDER BY (rentals_per_category / films_in_category::Numeric) DESC) AS rank_rentals_per_film
FROM view_ordered_by_rentals_per_category
ORDER BY rentals_per_film DESC;
```

SUMMARY TABLE:

The summary table, `daily_status_and_recommendation` is a regular table derived from the two view tables `view_ordered_by_rentals_per_film` and `view_ordered_by_films_in_category` and consists of the following six fields:

- `category_name` (varchar film category of the dvd)
- `rentals_per_film` (numeric value selected from view_ordered_by_rentals_per_film table)
- `rank_rentals_per_film` (integer value selected from view_ordered_by_rentals_per_film table)
- `inventory_rank` (integer value selected from view_ordered_by_films_in_category table)
- `demand_inventory_discrepancy` (a calculated numeric value of the difference between the rank_rentals_per_film and inventory_rank columns)
- `recommendation` (textual recommendation to the end user)

```sql
SELECT r.*, inventory_rank,
       (r.rank_rentals_per_film - inventory_rank) AS demand_inventory_discrepancy,
      (CASE WHEN (r.rank_rentals_per_film - inventory_rank) BETWEEN 3 AND 7 THEN 'purchase fewer'
      WHEN (r.rank_rentals_per_film - inventory_rank) BETWEEN -7 AND -3 THEN 'purchase more'
      WHEN (r.rank_rentals_per_film - inventory_rank) >= 8 THEN 'purchase far fewer'
      WHEN (r.rank_rentals_per_film - inventory_rank) <= -8 THEN 'purchase far more'
      ELSE 'no action'
      END
      ) AS Recommendation
INTO daily_status_and_recommendation
FROM view_ordered_by_rentals_per_film r
INNER JOIN view_ordered_by_films_in_category c
  ON r.category_name = c.category_name
ORDER BY demand_inventory_discrepancy;
```

A.4: custom-transformed field
The `daily_status_and_recommendation.recommendation` column (in the table above this paragraph) is a transformed textual recommendation with one of five string values based on the value of the demand_inventory_discrepancy column of the same table. A CASE WHEN statement is used to transform the numeric value to one of five textual action recommendations to be read by the end user of the table. The numeric value would not have been very helpful, so the text transformation makes the table more user friendly and action-oriented.

A.5. Business uses of the detailed and summary tables

The first table, `category_film_inventory_rental_view`, is a materialized view table generated by a join query of the four underlying tables. This table gathers all the raw data used by the remaining detailed tables and the summary table, and in this way it performs the role of an always updated data mart for the analytics of this report. It is therefore necessary that this table is never stale, so there will be triggers to update this table whenever a write query is performed on any of its underlying source tables. The advantage of being a materialized view is that the results of this query is cached, which makes it more performant when queried by the other views to generate the summary table.

```sql
SELECT * FROM category_film_inventory_rental_view;
```

The second table, `view_ordered_by_rentals_per_category`, is a view table whose main purpose is to generate the `rentals_per_category` column which will be pulled by the summary table and also used by the summary table to derive the discrepancy and recommendation columns.

```sql
SELECT * FROM view_ordered_by_rentals_per_category;
```

The third table, `view_ordered_by_films_in_category`, is a view table that contributes an important column for further use: the inventory_rank which is calculated by the films_in_category column. The inventory_rank will be used in the summary table to calculate the discrepancy and generate the final recommendation.

```sql
SELECT * FROM view_ordered_by_films_in_category;
```

The fourth table, `view_ordered_by_rentals_per_film`, provides the final missing piece for the summary table, which is the rank_rentals_per_film based values of the rentals_per_film column. The rank_rentals_per_film will be arithmetically paired with the inventory_rank in the summary table to calculate the discrepancy and generate the final recommendation.

```sql
SELECT * FROM view_ordered_by_rentals_per_film;
```

The summary table, `daily_status_and_recommendation` provides all the columns and values of interest to the end user, which is imagined to be the person responsible for purchases of new dvds or sales of dvds with little demand.
This person will thus view an overview of demand for films in each category, any discrepancy between the demand and inventory (for example if the most in-demand film category is far down the list in inventory rank), and a recommendation of buying more or fewer of a film based on this discrepancy, or no action if the two ranks match.

```sql
SELECT * FROM daily_status_and_recommendation;
```

A6. Frequency table refreshment
The system design of the tables in this report was deliberately made to ease maintenance and enhance performance.
The raw data table, `category_film_inventory_rental_view`, is designed as a materialized view table with update triggers so that it updates whenever an underlying table is updated. It will thus stay updated and performant (by storing its results in a cache) at all times.

The other three detailed tables are designed as view tables so that their underlying queries are executed on the always updated `category_film_inventory_rental_view` table. This alleviates the need to permanently store data from these table or to update them intermittently as they will always generate up-to-date tables.

The summary table, `daily_status_and_recommendation`, is therefore the only table that must be refreshed manually or with an automated cron job. I would have it refreshed daily every 24 hours after midnight. The advantage of this is that the query would take place when system traffic is at its lowest (assuming that users are not evenly distributed across all global timezones). Since recreating this table is a relatively time-consuming nested query using two other view tables (which trigger their queries), it is advantageous that this takes place outside of business hours so that the status and recommendation is available to the user at the beginning of each business day. If business days are Monday to Friday, then this job could be omitted Saturday and Sunday so that the Friday midnight report is available Monday morning.
The reason the cron job would run daily is that the person in charge of sales can be expected to make dvd purchase and/or sales decisions every day or on any given day, which means that this person will be able to make informed decisions based on the most recent data. An added advantage of daily refresh rates is that any purchases or sales the day before will be embedded in the updated data for the following day, thus avoiding making superflous, repeat bulk purchases of dvds for a single category due to past previous purchases not being accounted for. In the case where purchases decisions were to take place multiple times in a day and where previous purchases that day affects later purchases, then the script called by the cron job could be executed manually at decision time in order to generate an updated table.

B. Original code for function(s) in text format that perform transformations.

```sql
SELECT r.*, inventory_rank,
       (r.rank_rentals_per_film - inventory_rank) AS demand_inventory_discrepancy,
      (CASE WHEN (r.rank_rentals_per_film - inventory_rank) BETWEEN 3 AND 7 THEN 'purchase fewer'
      WHEN (r.rank_rentals_per_film - inventory_rank) BETWEEN -7 AND -3 THEN 'purchase more'
      WHEN (r.rank_rentals_per_film - inventory_rank) >= 8 THEN 'purchase far fewer'
      WHEN (r.rank_rentals_per_film - inventory_rank) <= -8 THEN 'purchase far more'
      ELSE 'no action'
      END
      ) AS Recommendation
INTO daily_status_and_recommendation
FROM view_ordered_by_rentals_per_film r
INNER JOIN view_ordered_by_films_in_category c
  ON r.category_name = c.category_name
ORDER BY demand_inventory_discrepancy;
```

C. Original SQL code of detailed and summary tables

```sql
CREATE OR REPLACE VIEW view_ordered_by_rentals_per_category AS
SELECT category_name,
      COUNT(DISTINCT film_id) AS films_in_category,
      COUNT(rental_id) AS rentals_per_category
FROM category_film_inventory_rental_view
GROUP BY category_name
ORDER BY rentals_per_category DESC;
```

```sql
CREATE OR REPLACE VIEW view_ordered_by_films_in_category AS
SELECT category_name,
      COUNT(DISTINCT film_id) AS films_in_category,
      RANK() OVER (ORDER BY COUNT(DISTINCT film_id) DESC) AS inventory_rank
FROM category_film_inventory_rental_view
GROUP BY category_name
ORDER BY films_in_category DESC;
```

```sql
CREATE OR REPLACE VIEW view_ordered_by_rentals_per_film AS
SELECT category_name,
      ROUND(rentals_per_category / films_in_category::Numeric, 2) AS rentals_per_film,
      RANK() OVER(ORDER BY (rentals_per_category / films_in_category::Numeric) DESC) AS rank_rentals_per_film
FROM view_ordered_by_rentals_per_category
ORDER BY rentals_per_film DESC;
```

<!-- Summary table -->

```sql
SELECT r.*, inventory_rank,
       (r.rank_rentals_per_film - inventory_rank) AS demand_inventory_discrepancy,
      (CASE WHEN (r.rank_rentals_per_film - inventory_rank) BETWEEN 3 AND 7 THEN 'purchase fewer'
      WHEN (r.rank_rentals_per_film - inventory_rank) BETWEEN -7 AND -3 THEN 'purchase more'
      WHEN (r.rank_rentals_per_film - inventory_rank) >= 8 THEN 'purchase far fewer'
      WHEN (r.rank_rentals_per_film - inventory_rank) <= -8 THEN 'purchase far more'
      ELSE 'no action'
      END
      ) AS Recommendation
INTO daily_status_and_recommendation
FROM view_ordered_by_rentals_per_film r
INNER JOIN view_ordered_by_films_in_category c
  ON r.category_name = c.category_name
ORDER BY demand_inventory_discrepancy;
```

D. Extraction of raw data needed for the detailed sections.

```sql
CREATE MATERIALIZED VIEW IF NOT EXISTS category_film_inventory_rental_view AS
SELECT category.name AS category_name,
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
```

E. Triggers

An added advantage of materialized view tables is ease of refreshing it. A simple `REFRESH MATERIALIZED VIEW` takes the place of a `DROP TABLE` followed by a `CREATE TABLE` query of regular tables.

For category and film_categories, any update should also trigger a refresh of the underlying material view.

```sql
CREATE OR REPLACE FUNCTION refresh_category_film_inventory_rental_view() RETURNS TRIGGER AS
$$
BEGIN
  REFRESH MATERIALIZED VIEW category_film_inventory_rental_view;

  RETURN new;
END;
$$
language plpgsql;
```

```sql
CREATE TRIGGER category_insert_delete
     AFTER INSERT OR DELETE OR UPDATE
     ON category
     EXECUTE PROCEDURE refresh_category_film_inventory_rental_view();
```

```sql
CREATE TRIGGER film_category_insert_delete
     AFTER INSERT OR DELETE OR UPDATE
     ON film_category
     EXECUTE PROCEDURE refresh_category_film_inventory_rental_view();
```

For the remaining tables, not every update should trigger a refresh so there will be separate update triggers for those.

```sql
CREATE TRIGGER film_insert_delete
     AFTER INSERT OR DELETE
     ON film
     EXECUTE PROCEDURE refresh_category_film_inventory_rental_view();
```

```sql
CREATE TRIGGER inventory_insert_delete
     AFTER INSERT OR DELETE
     ON inventory
     EXECUTE PROCEDURE refresh_category_film_inventory_rental_view();
```

```sql
CREATE TRIGGER rental_insert_delete
     AFTER INSERT OR DELETE
     ON rental
     EXECUTE PROCEDURE refresh_category_film_inventory_rental_view();
```

Update-only triggers

For updates on the film or inventory tables, only updates on the `film_id` column (the join predicate) will affect the summary results. This means that we don't want to trigger the same function on just any update of rows in these two tables, and also that we can create a trigger for each that calls the same function.
NB. If a film changes category, that change is captured in the already created film_category trigger.

```sql
CREATE OR REPLACE FUNCTION film_id_update_function() RETURNS TRIGGER AS
$$
BEGIN
  IF NEW.film_id <> OLD.film_id THEN
    REFRESH MATERIALIZED VIEW category_film_inventory_rental_view;
  END IF;
  RETURN NEW;
END;
$$
language plpgsql;
```

```sql
CREATE TRIGGER film_update
     AFTER UPDATE ON film
     FOR EACH ROW
     EXECUTE PROCEDURE film_id_update_function();
```

```sql
CREATE TRIGGER inventory_update
     AFTER UPDATE ON inventory
     FOR EACH ROW
     EXECUTE PROCEDURE film_id_update_function();
```

The last trigger will be an update trigger on the rental table. Here, an update on the `inventory_id` affects the number of rentals for that inventory item and by extension the film category that that inventory item is associated with. Since `rental_id` is a join_predicate, an update also needs to precipitate a join_table teardown and recreation.

```sql
CREATE OR REPLACE FUNCTION rental_update_function() RETURNS TRIGGER AS
$$
BEGIN
  IF NEW.inventory_id <> OLD.inventory_id OR NEW.rental_id <> OLD.rental_id THEN
      REFRESH MATERIALIZED VIEW category_film_inventory_rental_view;
  END IF;
  RETURN NEW;
END;
$$
language plpgsql;
```

```sql
CREATE TRIGGER rental_update
     AFTER UPDATE ON rental
     FOR EACH ROW
     EXECUTE PROCEDURE rental_update_function();
```

F. Original stored procedure to refresh summary table.
Since the detailed table is always updated via triggers, the procedure called by a scheduled cron job only needs to update the summary table.

```sql
CREATE PROCEDURE update_daily_status_and_recommendation()
LANGUAGE SQL
AS $$
DROP TABLE daily_status_and_recommendation;

SELECT r.*, inventory_rank,
       (r.rank_rentals_per_film - inventory_rank) AS demand_inventory_discrepancy,
      (CASE WHEN (r.rank_rentals_per_film - inventory_rank) BETWEEN 3 AND 7 THEN 'purchase fewer'
      WHEN (r.rank_rentals_per_film - inventory_rank) BETWEEN -7 AND -3 THEN 'purchase more'
      WHEN (r.rank_rentals_per_film - inventory_rank) >= 8 THEN 'purchase far fewer'
      WHEN (r.rank_rentals_per_film - inventory_rank) <= -8 THEN 'purchase far more'
      ELSE 'no action'
      END
      ) AS Recommendation
INTO daily_status_and_recommendation
FROM view_ordered_by_rentals_per_film r
INNER JOIN view_ordered_by_films_in_category c
  ON r.category_name = c.category_name
ORDER BY demand_inventory_discrepancy;
$$;
```

F.1 job scheduling tool
I would implement a daily linux cron job to run a sql script that calls this procedure, `update_daily_status_and_recommendation`, which tears down and recreates this table very 24 hours post midnight. This is straight forward to implement, the timing can easily be altered, and no further extensions are needed since cron daemons are native to the operating system.

H. No third-party code or references were made in this report.
