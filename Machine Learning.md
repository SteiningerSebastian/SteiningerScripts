# Types of Machine Learning
![Types of Machine Learning](https://github.com/user-attachments/assets/630d1b2d-0683-46ee-af9d-83751bab4b1f)
![Machine Learning Types visualization](https://github.com/user-attachments/assets/1a3e4dd8-2e1e-4eba-bf8d-3b0d0eb50193)

## Supervised Learning
In supervised learning, a model learns to predict the future based on what happened in the past.
Here, historical data with “ground-truth labels” (where there is an actual observed outcome for each input) is fed into supervised learning algorithms. So you might classify users who performed a certain behavior with a 1, and others with a 0.
The algorithm will identify correlations, patterns, and trends that are historically correlated with these ground-truth outcomes and then use them to predict the future.
Generally, you want your predictions to match ground-truth outcomes as frequently as possible – that is, you want your model to make as many correct predictions as it can. (Although it’s worth noting that accuracy is not the only worthy measure of a machine learning model.)
Example:
![Supervised learning visualization](https://github.com/user-attachments/assets/7dd43068-ad6c-4f48-b6ef-f32658a31e3f)

## Unsupervised Learning
In unsupervised learning, a model attempts to understand what happened in the past so it can be used to classify future outcomes.
Unsupervised learning makes it possible to uncover patterns in datasets where ground-truth labels are not available – that is, when records cannot be labeled with a certain target behavior (i.e., classified with a 0 or 1). The objective is to understand past outcomes, how they can be grouped or labeled, and what hidden patterns they hold. With that information in hand, future behavior can be labeled and predicted.
Here, you would run an unsupervised model where the algorithm doesn’t have a target (e.g., 1 or 0) but rather looks at the behavior of different customers or other entities and tells you which ones are behaving similarly to each other. It’s “unsupervised” because the model builder never tells the AI which classifications to perform.
However, you can manually assess the results and identify certain qualities that exist among groups. So while you may not be able to identify, say, people who are likely to commit fraudulent activity, you may identify classifications related to income level, shopping habits, etc.
Examples:
![Unsupervised Machine Learning visualization](https://github.com/user-attachments/assets/67b6d678-1ec0-47b9-9d73-730ce55dd0c5)

[![42Entwickler Merkmale](https://github.com/user-attachments/assets/43d073c0-4d81-41dd-8938-82e588f9b4a2)](https://youtu.be/h4QnleRrV9w?si=Zk736sfmiyopqROb&t=562)

## Reinforcement Learning
In reinforcement learning, a model uses trial and error to iteratively improve its approach to making decisions. The algorithm continually tries new things to learn how to behave better in the future.
Here, raw data (whether in real-time or in batches) serves as the input. The reinforcement learning algorithm learns by constantly seeking new approaches to a problem and improving upon approaches that maximize the defined “reward.” This type of learning is frequently used in robotics and gaming.

# Clustering
Clustering is a powerful technique that allows us to discover hidden structures and patterns within data. Imagine having a collection of data points, perhaps representing customers, images, or even biological samples. Without any prior knowledge of their categories, clustering algorithms step in to automatically group these points based on their inherent similarities. This process, known as unsupervised learning, aims to partition the data into meaningful clusters, where points within the same cluster are more alike than those in other clusters. By revealing these natural groupings, clustering provides valuable insights for tasks ranging from market segmentation and image analysis to anomaly detection and scientific discovery.

## Hierarchical Clustering
Hierarchical clustering is a powerful unsupervised machine learning method designed to construct a nested hierarchy of clusters from a given dataset. Unlike partitioning methods that produce a single partition of the data, hierarchical clustering yields a tree-like structure, known as a dendrogram, which visually represents the relationships between data points and clusters at various levels of granularity.

### The Algorithm
#### 1. Initialization
Initially, every data point is treated as its own individual cluster, resulting in 'n' clusters for 'n' data points.

#### 2. Distance Calculation
Calculate the pairwise distances between all clusters.
With either:
- Euclidean Distance sqrt(a²+b²)
or
- Manhatten Distance | a + b |

#### 3. Merging Clusters (Agglomerative Clustering)
Find the two closest clusters based on the distance/similarity metric. These clusters are merged,
by putting a representant in the middle of the two clusters, which is now a new cluster(point) and the other two get "eliminated".

#### 4. Update the Distance Matrix
Recalculate the distances or similarities between the newly formed and the remaining clusters.
This step reduces the number of clusters by one.

#### 5. Repeat Steps 3 and 4
Continue merging the closest clusters and updating the distance matrix until only one cluster remains. This process forms a hierarchy of clusters.

#### 6. Creating a Dendrogram
As clusters are merged, you can represent the hierarchical structure using a dendrogram. A dendrogram is a tree-like diagram visually showing clusters’ merging processes and relationships.

#### 7. Cutting the Dendrogram
You can cut the dendrogram at a certain level to obtain a specific number of clusters. The height at which you cut the dendrogram determines the number of clusters you get. Choosing the right level often involves domain knowledge or methods like the elbow method or silhouette analysis.

### Cluster Assignment
Once you’ve determined the desired number of clusters, you can assign each data point to its corresponding cluster based on the hierarchical structure you’ve created.

Hierarchical clustering is a versatile method because it doesn’t require you to specify the number of clusters in advance. It allows you to explore different levels of granularity in your data, from a few large clusters to many small clusters. Additionally, the dendrogram visually represents how data points are related, making it easier to interpret the results. However, hierarchical clustering can be computationally expensive for large datasets.

![image](https://github.com/user-attachments/assets/d9ec6299-e468-4457-a676-0afabd6cdfae)

Source: https://spotintelligence.com/2023/09/12/hierarchical-clustering-comprehensive-practical-how-to-guide-in-python/#How_Does_Hierarchical_Clustering_Work

> Clustering is a unsupervised learning technique used to find clusters.
> 
> Hierarchical Clustering is a type of clustering that takes a hierarchical approachh to identify cluster centroids.
