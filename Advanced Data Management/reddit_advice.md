## on project approach

1.  Passed in a little over a week, with a little over 7 hours logged into the virtual machine. This class really wasn't bad. The hardest part was understanding the project. Here is my advice. Aside from figuring out your business question, **write the paper portion last**. My plan certainly ended up changing a couple of times once I started actually working, and I needed to add more data members to my tables than I initially thought (mostly foreign keys to access tables I didn't think I would need). Second, **examine the database before you begin working**. Log into the lab environment, run some select queries, get a feel for what data types you're working with. **Pay attention to cardinality on the ERD!** Finally, people have suggested emailing your code to yourself to keep it backed up. I found a much easier way to back up my code. I simply opened Google Drive in the VM, created a new Doc, and kept my code updated there. I could easily access it outside of the VM and even go back and view previous changes with the features of google drive.

- it is **not required to call your Function, Trigger, or Stored Procedure**.

2. This course was tough in that it was vague, but once you got things going, it really went pretty quick. You are really only applying a couple of new portions of SQL in it. Everything else up to this point you should have done (creating tables, filling in tables, joins, etc.). Creating functions, procedures, and triggers are the new things, once you figure out how they work, everything starts to make so much more sense. Think my project was only about 150 lines or so.

3. My best advice is to **keep your question super simple** and to **watch all 3 videos**. There is one 50ish minute video on a general overview of the PA. One 40ish minute video on writing functions and a 1 hour 40 minute video on triggers and procedures. It also refreshes you on creating tables by joining more than 2 tables. That is really all you need to do the project. Oh **and one article in the material to answer the scheduling tool question**.

4. - Verify each query is correct with a follow up SELECT query

- My code, with many comments (to help with my narration, mostly) came out to only 125 lines, and the video was 5 minutes long.

## Business Ideas - As long as you can explain why it is relevant to the business, you're good.

Rentals per store

- to close low rental stores
- increase inventory in high rental stores
- Increase number of stores in cities with high rental-per-store figure

- aggregate: eg total sales per employee
- total rentals per store
- how many customers rented a certain movie
- how many times each movie was rented
- how long it has been since a customer has rented a DVD.
- which film category has the highest gross rentals per store,

## on recording

_downloaded the database, did the project on my PC, and then pasted all the SQL commands into the lab environment when finished. I wrote a comment above each SQL command stating what part of the rubric it performed with a short description of what the command would do. For the presentation, I wrote a script and read it as I executed each command. I didn't use my webcam - only a mic for the presentation. Just make sure you do everything outlined in the rubric. _

I had it all typed up already in pgAdmin4 when the video started. You can highlight sections of code and when you click execute, it only runs what you have highlighted. Section B is to create the tables, so I highlighted the create table code and executed it. Then I scrolled down until I could see all the section C code, highlighted it, then executed. I just did that for each section.
EDIT: Section D is kind of a two-parter. I highlighted and executed the code to create the function, then I highlighted and executed the code that actually used the function to transform the data.

_I set the camera to face me, but some say you don't have to be on the video. I commented my code and used the comments as my script to narrate the video and just said "This is the blah blah blah" while highlighting and running the code. Then, "If I run this select statement We'll see that the code did what it is supposed to do" while highlighting and running the select statement. _

_I set the camera towards me as I was explaining the code. I wrote statements as I would say them for the explanation in the video. Email the code to yourself.(easiest method). Past the code in your scratch pad in Postgres. Copy and paste as needed while explaining what each part does. I used my iPad to read the script I prepared while running the sql on my desktop. (You’re only recording yourself and the lab displaying the code as you build it and run it). _

_I didn’t show my video but only the screenshare of the postgress sql and executed my scripts one by one in sequence as outlined in the PA. Hope this helps! _

\*Okay so I just passed this course a couple days ago. For recording I used my webcam facing me, when talking about the code. I didn’t want to risk having my submission being returned.

I emailed myself my SQL code in the lab and pasted it in pg Admin. I commented it out using a multi line comment. Then I started recording. I made sure to move the comment symbol up after each section. Also I added a couple comments to help remember what I wanted to talk about, because I get nervous and I forget.\*

## Faults to avoid

The submission observes that update frequency can be accommodated to any business model, which is valid. A logical explanation of how the stored procedure could be automatically run on a schedule to ensure data freshness is not found.

- I just added 2 - 3 BS sentences about a data administrator calling the function every month or alternatively writing a script to automate the process so that the function is called automatically.

## Details vs summary table

Your detail table will be the raw data that makes up the aggregate data in the summary table. for example, if you ultimately want to know the total number of rentals pet month per category then the detail table will contain every single rental, film title, and category but the summary table will contain only the month, category, and count of rentals.

## Stored Procedure

You just create a procedure that deletes your detailed table and replace it with a new detailed table by including an insert into query. The stored procedure is for refreshing your detailed data when you put new data in
