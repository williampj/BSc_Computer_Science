import pandas as pd
from matplotlib import pyplot
from pandas.plotting import scatter_matrix
from sklearn import linear_model, metrics, model_selection
import re

url = 'https://raw.githubusercontent.com/williampj/general_study/master/housing_without_headers.csv'
headers = ["sqfeet", "beds", "baths", "price"]

df = pd.read_csv(url, names=headers)

x = df.values[:, 0:3]
y = df.values[:, 3]

regression_model = linear_model.LogisticRegression(solver='liblinear')
x_train, x_test, y_train, y_test = model_selection.train_test_split(x, y, test_size=0.25, train_size=0.75)
regression_model.fit(x_train, y_train)

def test_accuracy():
    x_train, x_test, y_train, y_test = model_selection.train_test_split(x, y, test_size=0.25, train_size=0.75)
    regression_model.fit(x_train, y_train)
    y_predict_regression = regression_model.predict(x_test)
    print(f'\n{metrics.accuracy_score(y_test, y_predict_regression)} was the accuracy score for this test')

def user_interface():
    # 20 - 3000
    valid_square_feet = False
    while not valid_square_feet:
        square_feet = input("\nHow many square feet do you require? (choose between 20 and 3000) ")
        if bool(re.match('^[0-9]+$', square_feet)) and 20 <= int(square_feet) <= 3000:
            valid_square_feet = True
            square_feet = int(square_feet)
        else:
            print("Sorry, that is not a valid number for square footage\n")

    valid_beds = False
    # beds = -1
    while not valid_beds:
      # 0-5 (0 is a studio)
        beds = input("\nHow many bedrooms? (choose between 0 and 5, 0 being a studio) ")
        if bool(re.match('^[0-5]{1}$', beds)): # and 0 <= int(beds) <= 5:
            valid_beds = True
            beds = int(beds)
        else:
            print("Sorry, that is not a valid number of bedrooms")

    valid_bathrooms = False
    # bathrooms = -1
    while not valid_bathrooms:
      # 1-3
        bathrooms = input("\nHow many bathrooms? (choose between 1 and 3) ")
        if bool(re.match('^[1-3]{1}$', bathrooms)): # and 1 <= int(bathrooms) <= 3:
            valid_bathrooms = True
            bathrooms = int(bathrooms)
        else:
            print("Sorry, that is not a valid number of bathrooms")
    return [square_feet, beds, bathrooms]

# 4 Descriptive methods:
def descriptive_methods():
# Bar Plot
    df['price'].value_counts().plot.bar(title='Apartment count by price', x="Price", y="count")
# Histogram
    df.hist()
# Boxpolot of square feet by price
    df.boxplot(column='sqfeet', by='price', grid=True, vert=False)
    pyplot.show()

def predict_user_rental_price():
    user_inputs = user_interface()
    prediction_string = regression_model.predict([user_inputs])[0][0:-1]
    prediction_price = int(float(prediction_string) * 1000)
    print("\nYou should expect to budget $" + str(prediction_price) + ", +/- a few hundred dollars depending on other factors\n")

def run_program():
    continue_choice = True
    while continue_choice:

        valid_choice = False
        while not valid_choice:
            user_choice = input("""
            Please select an option between 1-3:

            1: Predict rental price
            2: Generate descriptive visuals of the NYC rental market
            3: Test the accuracy of the ML model

            """)
            if bool(re.match('^[1-3]{1}$', user_choice)):
                valid_choice = True
                user_choice = int(user_choice)
            else:
                print("\nSorry, that is not a valid choice\n")

        if user_choice == 1:
            predict_user_rental_price()
        elif user_choice == 2:
            descriptive_methods()
        else: # choice must have been 3
            test_accuracy()

        continue_input = input("\nWould you like to continue? ('y' for yes, any other key to quit) ").lower()
        if not bool(re.match('^(y|yes)$', continue_input)):
            continue_choice = False

def goodbye_message():
    print("\nThank you for using the NYC rental price predictor. Goodbye!\n")

run_program()

goodbye_message()




