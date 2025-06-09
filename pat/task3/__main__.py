import pandas as pd
import numpy as np
from typing import NamedTuple
from scipy.io import loadmat
from sklearn.decomposition import PCA
from sklearn.svm import SVC
from sklearn.model_selection import GridSearchCV
from sklearn.utils import shuffle
from skimage.feature import hog
from sklearn.metrics import accuracy_score
from random import choice
import argparse
import os

class DataSet(NamedTuple):
    train: np.ndarray
    test: np.ndarray

def dirpath(dir: str = '.') -> str:
    return os.path.normpath(os.path.join(os.path.dirname(__file__), dir))

def load_data() -> DataSet:
    data_dir = dirpath('data')
    train_data = loadmat(os.path.join(data_dir, 'train_data.mat'))['train']
    test_data = loadmat(os.path.join(data_dir, 'test_data.mat'))['test']
    return DataSet(train_data, test_data)

if __name__ == '__main__':
    parser = argparse.ArgumentParser(
        prog='pic-recog',
        description='Run recognition on the given train & test dataset with svm model, designed for nwpu parttern recognition course [TASK 3]'
    )
    parser.add_argument('--disable-mixup', default=False, action='store_true', help='disable train samples mixup')
    parser.add_argument('--mixup', default=0.3, type=float, help='specify samples mixup coefficient')
    parser.add_argument('--feature-extractor', '-c', default='hog', type=str, choices=['hog', 'pca', 'none'], help='specify samples mixup coefficient')
    args = parser.parse_args()

    try:
        d = load_data()
    except:
        print('error: expect train_data.mat & test_data.mat under ./data directory')
        exit()

    mixup = args.mixup
    enable_mixup = not args.disable_mixup
    feature_extractor = args.feature_extractor

    nr_target, nr_each_targets, w, h = d.train.shape
    nr_each_train = int(nr_each_targets * 0.75)

    X_train = d.train[:,:nr_each_train,:,:].reshape(nr_target * nr_each_train, -1)
    X_test = d.train[:,nr_each_train:,:,:].reshape(nr_target * (nr_each_targets - nr_each_train), -1)
    X_pred = d.test.reshape(d.test.shape[0], -1)

    y_train = np.array([[i + 1] * nr_each_train for i in range(nr_target)]).flatten()
    y_test = np.array([[i + 1] * (nr_each_targets - nr_each_train) for i in range(nr_target)]).flatten()

    if enable_mixup:
        X_mixup = X_train.copy()
        for i in range(len(X_mixup)):
            X_mixup[i] = (1.0 - mixup) * X_train[i] + mixup * choice(X_train)
        X_train = np.append(X_train, X_mixup, axis=0)
        y_train = np.append(y_train, y_train, axis=0)

    match feature_extractor:
        case 'hog':
            X_train = np.array([hog(X.reshape(w, h), pixels_per_cell=(5, 5)) for X in X_train])
            X_test = np.array([hog(X.reshape(w, h), pixels_per_cell=(5, 5)) for X in X_test])
            X_pred = np.array([hog(X.reshape(w, h), pixels_per_cell=(5, 5)) for X in X_pred])
        case 'pca':
            pca_decomp = PCA(n_components=0.95, whiten=True, random_state=42)
            pca_decomp.fit_transform(np.concatenate((X_train / 255, X_test), axis=0))
            X_train = pca_decomp.transform(X_train)
            X_test = pca_decomp.transform(X_test)
            X_pred = pca_decomp.transform(X_pred)
        case 'none':
            pass
        case _:
            print(f'error: unsupported feature extractor {feature_extractor}')
            exit()

    N = 50
    param_grid = {
        'kernel': ['linear', 'poly', 'rbf', 'sigmoid'],
        'C': list(map(lambda t: (10 ** (t / N)) ** 2, range(-N, N + 1))),
    }

    model = SVC()
    model = GridSearchCV(model, param_grid, cv=10, n_jobs=-1)
    model.fit(*shuffle(X_train, y_train, random_state=42))
    print(f'info: best svm model with C={model.best_params_["C"]} kernel={model.best_params_["kernel"]}')
    print(f'info: train score {model.best_score_:.4f}')
    print(f'info: train accuracy {accuracy_score(y_train, model.predict(X_train)):.4f} test accuracy {accuracy_score(y_test, model.predict(X_test)):.4f}')

    y_pred = model.predict(X_pred)

    output_dir = dirpath('output')
    os.makedirs(output_dir, exist_ok=True)
    df_resp = pd.DataFrame({
        '图片': list(range(1, len(y_pred) + 1)),
        '预测结果': y_pred,
    })
    df_resp.to_csv(os.path.join(output_dir, 'submission.csv'), encoding='gbk', index=False)
