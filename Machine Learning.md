TODO: to get started look at 'Getting Started.md'

Hierarchical
Hierarchical clustering is a technique that hierarchizes data points or objects into clusters. It starts with each data point as its cluster and then iteratively merges or splits clusters until a specific criterion is met. 

1. Initialization:

Begin with each data point as a separate cluster. In other words, if you have ‘n’ data points, you start with ‘n’ clusters, each containing one data point.
2. Distance Calculation:

Calculate the pairwise distances or similarities between all clusters.
With either:
•	Euclidean Distance	(sqrt(a²+b²))
or
•	Manhatten Distance	| a + b |
for the whole progress

3. Merging Clusters (Agglomerative Clustering):

Find the two closest clusters based on the distance/similarity metric. These clusters are merged,
by putting a representant in the middle of the two clusters, which is now a new cluster(point) and the other two get "eliminated"
4. Update the Distance Matrix:

Recalculate the distances or similarities between the newly formed and the remaining clusters.
This step reduces the number of clusters by one.
5. Repeat Steps 3 and 4:

Continue merging the closest clusters and updating the distance matrix until only one cluster remains. This process forms a hierarchy of clusters.
6. Creating a Dendrogram:

As clusters are merged, you can represent the hierarchical structure using a dendrogram. A dendrogram is a tree-like diagram visually showing clusters’ merging processes and relationships.
7. Cutting the Dendrogram:

You can cut the dendrogram at a certain level to obtain a specific number of clusters. The height at which you cut the dendrogram determines the number of clusters you get. Choosing the right level often involves domain knowledge or methods like the elbow method or silhouette analysis.
8. Cluster Assignment:

Once you’ve determined the desired number of clusters, you can assign each data point to its corresponding cluster based on the hierarchical structure you’ve created.
Hierarchical clustering is a versatile method because it doesn’t require you to specify the number of clusters in advance. It allows you to explore different levels of granularity in your data, from a few large clusters to many small clusters. Additionally, the dendrogram visually represents how data points are related, making it easier to interpret the results. However, hierarchical clustering can be computationally expensive for large datasets.

![image](https://github.com/user-attachments/assets/d9ec6299-e468-4457-a676-0afabd6cdfae)
Source: https://spotintelligence.com/2023/09/12/hierarchical-clustering-comprehensive-practical-how-to-guide-in-python/#How_Does_Hierarchical_Clustering_Work
