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
- Manhatten Distance abs(a + b )

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
