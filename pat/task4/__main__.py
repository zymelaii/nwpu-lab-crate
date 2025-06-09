import pandas as pd
import numpy as np
from typing import NamedTuple
from sklearn.model_selection import GridSearchCV
from sklearn.feature_extraction import DictVectorizer
from sklearn.tree import DecisionTreeClassifier
import os
import argparse
import re

class DataSet(NamedTuple):
    train: pd.DataFrame
    test: pd.DataFrame

def dirpath(dir: str = '.') -> str:
    return os.path.normpath(os.path.join(os.path.dirname(__file__), dir))

def load_data() -> DataSet:
    data_dir = dirpath('data')
    train_data = pd.read_csv(os.path.join(data_dir, 'train.csv'))
    test_data = pd.read_csv(os.path.join(data_dir, 'test.csv'))
    return DataSet(train_data, test_data)

if __name__ == '__main__':
    parser = argparse.ArgumentParser(
        prog='tree-pred',
        description='Run prediction on the given train & test dataset with decision tree, designed for nwpu parttern recognition course [TASK 4]'
    )
    parser.add_argument('--max-depth', '-d', default=32, type=int, help='max depth of the decision tree')
    parser.add_argument('--feature-list', '-f', default='Pclass,Sex,Age', type=str, help='feature list to be decided on')
    args = parser.parse_args()

    max_depth = max(args.max_depth, 8)
    pat = re.compile(r'(\d+)')
    active_feature_names = list(set(args.feature_list.split(',')))

    if len(active_feature_names) == 0:
        print('error: no features provided')
        exit()

    try:
        d = load_data()
    except:
        print('error: expect train.csv & train.csv under ./data directory')
        exit()

    X = d.train[active_feature_names].copy()
    y = d.train['Survived'].to_numpy()

    X_pred = d.test[active_feature_names].copy()
    I_pred = d.test['PassengerId'].to_numpy()

    if 'Ticket' in X:
        X['Ticket'] = list(map(lambda e: e and float(e.group(1)), map(lambda e: pat.search(str(e)), X['Ticket'])))
        X_pred['Ticket'] = list(map(lambda e: e and float(e.group(1)), map(lambda e: pat.search(str(e)), X_pred['Ticket'])))
        X.fillna({'Ticket': X['Ticket'].mean()}, inplace=True)
        X_pred.fillna({'Ticket': X_pred['Ticket'].mean()}, inplace=True)

    if 'Age' in X:
        X.fillna({'Age': X['Age'].mean()}, inplace=True)
        X_pred.fillna({'Age': X_pred['Age'].mean()}, inplace=True)

    encoder = DictVectorizer(sparse=False)
    X = encoder.fit_transform(X.to_dict(orient="records"))
    X_pred = encoder.transform(X_pred.to_dict(orient="records"))

    model = DecisionTreeClassifier(random_state=114514)
    model = GridSearchCV(model, {
        'max_depth': list(range(1, max_depth)),
    }, cv=10)
    model.fit(X, y)

    importances = list(zip(encoder.get_feature_names_out(), model.best_estimator_.feature_importances_))
    feature_importance = {}
    for k, v in importances:
        feature = k.split('=')[0]
        feature_importance.setdefault(feature, 0.0)
        feature_importance[feature] += v

    print(f'info: train score {model.best_score_} depth: {model.best_estimator_.get_depth()}')
    print(f'info: train accuracy {model.score(X, y):.4f}')
    print(f'info: feature importances: {" ".join(map(lambda e: f"{e[0]}={e[1]:.4f}", feature_importance.items()))}')

    y_pred = model.predict(X_pred)

    output_dir = dirpath('output')
    os.makedirs(output_dir, exist_ok=True)
    df_resp = pd.DataFrame({
        'PassengerId': I_pred,
        'Survived': y_pred,
    })
    df_resp.to_csv(os.path.join(output_dir, 'submission.csv'), index=False)
