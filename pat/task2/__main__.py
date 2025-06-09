import pandas as pd
import numpy as np
from typing import NamedTuple
import seaborn as sns
import matplotlib.pyplot as plt
from sklearn.metrics import confusion_matrix, classification_report, roc_auc_score
from sklearn.preprocessing import StandardScaler
from sklearn.linear_model import Perceptron, LogisticRegression
from imblearn.over_sampling import SMOTE
from collections import Counter
import os
import argparse

class DataSet(NamedTuple):
    train: np.ndarray #<! [Xs..., Y, C]
    test: np.ndarray  #<! [Xs..., Y]

    def class_dist(self) -> dict[int, int]:
        return dict(Counter(self.train[:,-1]))

def dirpath(dir: str = '.') -> str:
    return os.path.normpath(os.path.join(os.path.dirname(__file__), dir))

def load_data() -> DataSet:
    data_dir = dirpath('data')
    train_data = pd.read_csv(os.path.join(data_dir, 'train_set.csv')).to_numpy()
    test_data = pd.read_csv(os.path.join(data_dir, 'test_set.csv')).to_numpy()[:,1:-1]
    return DataSet(train_data, test_data)

if __name__ == '__main__':
    parser = argparse.ArgumentParser(
        prog='classify',
        description='Run linear classification on the given train & test dataset with perceptron or logistic regression, designed for nwpu parttern recognition course [TASK 2]'
    )
    parser.add_argument('--no-normalized', '-n', default=False, action='store_true', help='disable nomalization on feature')
    parser.add_argument('--model', '-t', default='logistic', type=str, choices=['perceptron', 'logistic'], help='specify the classification model')
    args = parser.parse_args()

    model_name = args.model
    should_normalize = not args.no_normalized
    enable_smote = False

    try:
        d = load_data()
    except:
        print('error: expect train_set.csv & test_set.csv under ./data directory')
        exit()

    cls_dist = d.class_dist()
    if len(cls_dist) != 2:
        print('error: not a binary classification')
        exit()

    na, nb = cls_dist.values()
    if na < nb:
        na, nb = nb, na
    cls_ratio = na / nb
    if cls_ratio > 4:
        print('info: sample distribution is out of balance, oversampling will be enabled')
        enable_smote = True

    train = d.train.copy()
    test = d.test.copy()

    if should_normalize:
        train[:,-2] = StandardScaler().fit_transform(train[:,-2].reshape(-1, 1))[0]
        test[:,-1] = StandardScaler().fit_transform(test[:,-1].reshape(-1, 1))[0]

    X, Y = train[:,:-1], train[:,-1]
    if enable_smote:
        smote = SMOTE(random_state=38)
        X, Y = smote.fit_resample(X, Y)

    match model_name:
        case 'perceptron':
            model = Perceptron(
                penalty='l2',
                alpha=0.001,
                max_iter=1000,
                early_stopping=True,
                random_state=38,
            )
        case 'logistic':
            model = LogisticRegression(
                penalty='l2',
                max_iter=1000,
                random_state=38,
            )
        case _:
            print(f'error: unsupported model type: {model_name}')
            exit()

    model.fit(X, Y)
    print(f'info: train score: {model.score(X, Y)}')
    print(f'info: classification report:\n\n{classification_report(Y, model.predict(X))}')
    if hasattr(model, 'predict_proba'):
        print(f'info: roc-auc score: {roc_auc_score(Y, model.predict_proba(X)[:,1])}')

    pred = model.predict(test)

    output_dir = dirpath('output')
    os.makedirs(output_dir, exist_ok=True)

    sns.heatmap(confusion_matrix(Y, model.predict(X)), annot=True, fmt='d')
    plt.savefig(os.path.join(output_dir, f'{model_name}.png'))

    df_resp = pd.DataFrame({
        '序号': list(range(1, len(pred) + 1)),
        'Class': pred,
    })
    df_resp.to_csv(os.path.join(output_dir, 'submit.csv'), encoding='gbk', index=False)
