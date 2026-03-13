import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns
from sklearn.datasets import load_wine
from sklearn.model_selection import train_test_split

wine = load_wine()
X = wine.data
y = wine.target
feature_names = wine.feature_names
target_names = wine.target_names

df = pd.DataFrame(X, columns=feature_names)
df['target'] = y

print("First five rows of the Wine dataset:")
print(df.head())

print("\nSummary statistics:")
print(df.describe())

np.random.seed(42) 
selected_features = np.random.choice(feature_names, size=9, replace=False)

df['target_name'] = df['target'].map({i: name for i, name in enumerate(target_names)})

# pair plot:
sns.pairplot(df, vars=selected_features, hue='target_name', palette='viridis', 
             plot_kws={'alpha': 0.6, 's': 80, 'edgecolor': 'k'})
plt.suptitle('Pair Plot of Wine Features by Class', y=1.02, fontsize=16)
plt.tight_layout()
plt.savefig('wine_pairplot.png', bbox_inches='tight', dpi=300) 
plt.show()

