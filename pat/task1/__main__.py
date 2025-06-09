from typing import Tuple, List, NamedTuple
import os
import csv
import argparse
import numpy as np
import matplotlib.pyplot as plt
from sklearn.preprocessing import PolynomialFeatures
from sklearn.linear_model import LinearRegression
from sklearn.metrics import mean_squared_error
from sklearn.pipeline import Pipeline, make_pipeline

Point = Tuple[float, float]
PointSet = List[Point]

class DataSet(NamedTuple):
    train: PointSet
    test: PointSet

def dirpath(dir: str = '.') -> str:
    return os.path.normpath(os.path.join(os.path.dirname(__file__), dir))

def load_data() -> DataSet:
    data_dir = dirpath('data')
    with open(os.path.join(data_dir, 'train_data.csv'), 'r') as f:
        train_data: PointSet = list(map(lambda e: [float(e[0]), float(e[1])], [e for e in csv.reader(f)][1:]))
    with open(os.path.join(data_dir, 'test_data.csv'), 'r') as f:
        test_data: PointSet = list(map(lambda e: [float(e[0]), float(e[1])], [e for e in csv.reader(f)][1:]))
    return DataSet(train_data, test_data)

def fit_model(deg: int, x: List[float], y: List[float]) -> Pipeline:
    model = make_pipeline(
        PolynomialFeatures(degree=deg, include_bias=False),
        LinearRegression(),
    )
    model.fit(np.reshape(x, (-1, 1)), y)
    return model

def make_poly_formula(model: Pipeline) -> Tuple[str, List[float]]:
    feat: PolynomialFeatures = model.named_steps['polynomialfeatures']
    var = ['', *feat.get_feature_names_out(input_features=['x'])]
    r: LinearRegression = model.named_steps['linearregression']
    coef = [r.intercept_, *r.coef_]
    coef_t = list(map(lambda e: int(e * 100) / 100, coef))
    formula = ''.join(reversed(([f'{c:+}{v}' for v, c in zip(var, coef_t) if c != 0.0])))
    return formula if formula[0] != '+' else formula[1:], coef

def calc_k_fold_mse(degree: int, x: List[float], y: List[float], k=10) -> float:
    mse_sum = 0.0
    n_group = len(x) // k
    print(f'degree: {degree}')
    for i in range(k):
        x_train, x_test = x[:i*n_group] + x[(i+1)*n_group:], x[i*n_group:(i+1)*n_group]
        y_train, y_test = y[:i*n_group] + y[(i+1)*n_group:], y[i*n_group:(i+1)*n_group]
        print(mean_squared_error(y_test, fit_model(degree, x_train, y_train).predict(np.reshape(x_test, (-1, 1)))))
        mse_sum += mean_squared_error(y_test, fit_model(degree, x_train, y_train).predict(np.reshape(x_test, (-1, 1))))
    return mse_sum / k

if __name__ == '__main__':
    parser = argparse.ArgumentParser(
        prog='poly-fit',
        description='Run poly regression on the given train & test dataset, designed for nwpu parttern recognition course [TASK 1]'
    )
    parser.add_argument('--min-deg', '-m', default=1, type=int, help='min degree to search for')
    parser.add_argument('--max-deg', '-n', default=10, type=int, help='max degree to search for')
    parser.add_argument('--cross-validate', default=False, action='store_true', help='train with 10-fold cross validation')
    args = parser.parse_args()

    d = load_data()

    x, y = zip(*sorted(d.train, key=lambda e: e[0]))
    x_t, y_t = zip(*sorted(d.test, key=lambda e: e[0]))

    X = np.reshape(x, (-1, 1))
    X_t = np.reshape(x_t, (-1, 1))

    min_deg = args.min_deg
    max_deg = args.max_deg
    enable_cross_validation = args.cross_validate

    if enable_cross_validation:
        mse_list = [calc_k_fold_mse(deg, x, y, k=10) for deg in range(min_deg, max_deg + 1)]
    else:
        mse_list = [mean_squared_error(y_t, fit_model(deg, x, y).predict(X_t)) for deg in range(min_deg, max_deg + 1)]

    i_best_mse = np.argmin(mse_list)
    best_degree = i_best_mse + 1

    model = fit_model(best_degree, x, y)
    formula, poly_coef = make_poly_formula(model)

    resp = f'''Best Degree: {best_degree}
MSE: {mse_list[i_best_mse]}
Formula (Approximate): y={formula}
Coef: {list(map(float, poly_coef))}'''

    print(resp)

    x_c = np.linspace(min(*x, *x_t), max(*x, *x_t), 512)

    plt.figure(figsize=(12, 8))
    plt.scatter(x, y, c='#2c7ad6', marker='.', label='train')
    plt.scatter(x_t, y_t, c='#ce723b', marker='x', label='test')
    plt.plot(x_c, model.predict(x_c.reshape(-1, 1)), c='#f66129', label=f'fitted curve - {formula}')
    plt.legend()

    print(model.score(X_t, y_t))

    output_dir = dirpath('output')
    os.makedirs(output_dir, exist_ok=True)
    plt.savefig(f'{output_dir}/curve.png')
    plt.close()

    with open(f'{output_dir}/coef.txt', 'w') as f:
        f.write(resp)
