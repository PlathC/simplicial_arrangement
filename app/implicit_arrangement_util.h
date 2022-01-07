#pragma once
#include <simplicial_arrangement/simplicial_arrangement.h>
#include <string>

#include <Eigen/Core>

using namespace simplicial_arrangement;



// a polygon from the iso-surface of implicit function
struct IsoFace
{
    // a list of polygon's vertex indices (index into some global list of iso-vertices)
    std::vector<size_t> vert_indices;
    // the local index of this polygon in all the tets that contains it
    // each pair is (tet_Id, tet_face_Id)
    std::vector<std::pair<size_t, size_t>> tet_face_indices;
    // list of indices of bounding iso-edges (index into a global list of iso-edges)
    std::vector<size_t> edge_indices;
};

// vertex of isosurface
struct IsoVert
{
    // the tet containing the IsoVert
    size_t tet_index;
    // the index of IsoVert in tet.vertices
    size_t tet_vert_index;
    // minimal simplex that contains the IsoVert
    size_t simplex_size; // 1: point, 2: edge, 3: triangle, 4: tetrahedron
    // index into a list of tet vertices
    std::array<size_t, 4> simplex_vert_indices;
    // list of implicit functions whose isosurfaces pass IsoVert (indexed into a global list of
    // implicit functions)
    std::array<size_t, 3> func_indices;
};

struct IsoEdge
{
    size_t v1;
    size_t v2;
    // each pair is (iso_face_Id, edge_face_Id)
    // iso_face_Id: face index in the global list of iso-faces
    // edge_face_Id: edge index in the iso-face
    std::vector<std::pair<size_t, size_t>> face_edge_indices;
};

struct Simplicial_Cell
{
    // the tetrahedron containing the cell
    size_t tet_Id;
    // the index of cell in the tetrahedron
    size_t tet_cell_Id;
    // is_iso_face[i] == true if face i of the cell is on isosurface
    std::vector<bool> is_iso_face;
    // if is_iso_face[i], face_info[i] is the index of the patch which the iso-face belongs to
    // else, face_info[i] is the index of the simplicial cell on the other side of the face (if
    // there is no simplicial cell on the other side, face_info[i] == None)
    std::vector<size_t> face_info;
};

bool load_tet_mesh(const std::string &filename,
    std::vector<std::array<double, 3>> &pts,
    std::vector<std::array<size_t, 4>> &tets);

bool save_result(const std::string& filename,
    const std::vector<std::array<double, 3>>& iso_pts,
    const std::vector<IsoFace>& iso_faces,
    const std::vector<std::vector<size_t>>& patches,
    const std::vector<IsoEdge>& iso_edges,
    const std::vector<std::vector<size_t>>& chains,
    const std::vector<std::vector<size_t>>& non_manifold_edges_of_vert,
    const std::vector<std::vector<std::pair<size_t, int>>>& half_patch_list,
    const std::vector<std::vector<size_t>>& arrangement_cells);

bool save_result_mini(const std::string& filename,
    const std::vector<std::array<double, 3>>& iso_pts,
    const std::vector<IsoFace>& iso_faces,
    const std::vector<std::vector<size_t>>& patches,
    const std::vector<std::vector<size_t>>& arrangement_cells);

// save a list of iso-meshes
bool save_iso_mesh_list(const std::string& filename,
    const std::vector<std::vector<std::array<double, 3>>>& iso_pts_list,
    const std::vector<std::vector<IsoFace>>& iso_faces_list);


// save a triangle mesh
bool save_tri_mesh(const std::string& filename,
    const std::vector<std::array<double, 3>> &verts,
    const std::vector<std::array<size_t, 3>> &tris);

// extract the boundary triangle mesh of a tet mesh
// assume: the tet mesh represents a simply-connected 3D volume
// assume: the triangles of the boundary mesh don't need to be consistently oriented
// input:
//  verts: num_vert * 3, tet mesh vertices
//  tets: num_tet * 4, tet's corner vertices' indices
// output:
//  boundary_verts: num_boundary_vert * 3, boundary vertices
//  boundary_faces: num_boundary_face * 3, boundary triangle's vertex indices
void extract_tet_boundary_mesh(
    const std::vector<std::array<double, 3>> &verts,
    const std::vector<std::array<size_t, 4>> &tets,
    std::vector<std::array<double, 3>> &boundary_verts, std::vector<std::array<size_t,3>> &boundary_faces);

// save a triangle mesh
bool save_tri_mesh(const std::string& filename,
    const std::vector<std::array<double, 3>> &verts,
    const std::vector<std::array<size_t, 3>> &tris);

// save a list of triangle meshes
bool save_tri_mesh_list(const std::string& filename,
    const std::vector<std::vector<std::array<double, 3>>> &verts_list,
    const std::vector<std::vector<std::array<size_t, 3>>> &tris_list);

// extract the boundary triangle mesh of a tet mesh
// assume: the tet mesh represents a simply-connected 3D volume
// assume: the triangles of the boundary mesh don't need to be consistently oriented
// input:
//  verts: num_vert * 3, tet mesh vertices
//  tets: num_tet * 4, tet's corner vertices' indices
// output:
//  boundary_verts: num_boundary_vert * 3, boundary vertices
//  boundary_faces: num_boundary_face * 3, boundary triangle's vertex indices
void extract_tet_boundary_mesh(
    const std::vector<std::array<double, 3>> &verts,
    const std::vector<std::array<size_t, 4>> &tets,
    std::vector<std::array<double, 3>> &boundary_verts, std::vector<std::array<size_t,3>> &boundary_faces);

// given the list of vertex indices of a face, return the unique key of the face: (smallest vert Id,
// second smallest vert Id, largest vert Id) assume: face_verts is a list of non-duplicate natural
// numbers, with at least three elements.
void compute_iso_face_key(const std::vector<size_t>& face_verts, std::array<size_t, 3>& key);

// extract iso-mesh (topology only) and create map: local index --> global index
void extract_iso_mesh(const std::vector<bool>& has_isosurface,
    const std::vector<Arrangement<3>>& cut_results,
    const Eigen::Matrix<size_t, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> &func_in_tet,
    const Eigen::VectorXi &num_func_in_tet,
    const std::vector<std::array<size_t, 4>>& tets,
    std::vector<IsoVert>& iso_verts,
    std::vector<IsoFace>& iso_faces,
    std::vector<std::vector<size_t>>& global_vId_of_tet_vert,
    std::vector<std::vector<size_t>>& iso_fId_of_tet_face);

// extract iso-mesh (topology only)
void extract_iso_mesh_pure(const std::vector<bool>& has_isosurface,
    const std::vector<Arrangement<3>>& cut_results,
    const Eigen::Matrix<size_t, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> &func_in_tet,
    const Eigen::VectorXi &num_func_in_tet,
    const std::vector<std::array<size_t, 4>>& tets,
    std::vector<IsoVert>& iso_verts,
    std::vector<IsoFace>& iso_faces);

// extract iso-mesh from marching tet (topology only)
void extract_iso_mesh_marching_tet(const std::vector<bool>& has_isosurface,
    const std::vector<Arrangement<3>>& cut_results,
    const std::vector<std::array<size_t, 4>>& tets,
    std::vector<IsoVert>& iso_verts,
    std::vector<IsoFace>& iso_faces);

// compute xyz coordinates of iso-vertices
void compute_iso_vert_xyz(
    const std::vector<IsoVert> &iso_verts, 
    const Eigen::MatrixXd &funcVals,
    const std::vector<std::array<double, 3>> &pts,
    std::vector<std::array<double, 3>>& iso_pts);

// compute xyz coordinates of iso-vertices from marching tet
void compute_iso_vert_xyz_marching_tet(const std::vector<IsoVert>& iso_verts,
    const std::vector<double>& funcVals,
    const std::vector<std::array<double, 3>>& pts,
    std::vector<std::array<double, 3>>& iso_pts);

// compute iso-edges and edge-face connectivity
void compute_iso_edges(std::vector<IsoFace>& iso_faces, std::vector<IsoEdge>& iso_edges);

// group iso-faces into patches
void compute_patches(const std::vector<IsoFace>& iso_faces,
    const std::vector<IsoEdge>& iso_edges,
    std::vector<std::vector<size_t>>& patches);


// group non-manifold iso-edges into chains
void compute_chains(const std::vector<IsoEdge>& iso_edges,
    const std::vector<std::vector<size_t>>& non_manifold_edges_of_vert,
    std::vector<std::vector<size_t>>& chains);


// compute neighboring pair of half-faces around an iso-edge in a tetrahedron
// pair<size_t, int> : pair (iso-face index, iso-face orientation)
void compute_face_order_in_one_tet(const Arrangement<3>& tet_cut_result,
    const std::vector<IsoFace>& iso_faces,
    const IsoEdge& iso_edge,
    std::vector<std::pair<size_t, int>>& ordered_faces);


void compute_arrangement_cells(size_t num_patch,
    const std::vector<std::vector<std::pair<size_t, int>>>& half_patch_list,
    std::vector<std::vector<size_t>>& arrangement_cells);


// compute barycentric coordinate of Point (intersection of three planes)
// Point in tet cell
// template <typename Scalar>
inline void compute_barycentric_coords(const std::array<double, 4>& plane1,
    const std::array<double, 4>& plane2,
    const std::array<double, 4>& plane3,
    std::array<double, 4>& bary_coords)
{
    double n1 = plane1[3] * (plane2[2] * plane3[1] - plane2[1] * plane3[2]) +
                plane1[2] * (plane2[1] * plane3[3] - plane2[3] * plane3[1]) +
                plane1[1] * (plane2[3] * plane3[2] - plane2[2] * plane3[3]);
    double n2 = plane1[3] * (plane2[0] * plane3[2] - plane2[2] * plane3[0]) +
                plane1[2] * (plane2[3] * plane3[0] - plane2[0] * plane3[3]) +
                plane1[0] * (plane2[2] * plane3[3] - plane2[3] * plane3[2]);
    double n3 = plane1[3] * (plane2[1] * plane3[0] - plane2[0] * plane3[1]) +
                plane1[1] * (plane2[0] * plane3[3] - plane2[3] * plane3[0]) +
                plane1[0] * (plane2[3] * plane3[1] - plane2[1] * plane3[3]);
    double n4 = plane1[2] * (plane2[0] * plane3[1] - plane2[1] * plane3[0]) +
                plane1[1] * (plane2[2] * plane3[0] - plane2[0] * plane3[2]) +
                plane1[0] * (plane2[1] * plane3[2] - plane2[2] * plane3[1]);
    double d = n1 + n2 + n3 + n4;
    //
    bary_coords[0] = n1 / d;
    bary_coords[1] = n2 / d;
    bary_coords[2] = n3 / d;
    bary_coords[3] = n4 / d;
}

// Point on tet face
inline void compute_barycentric_coords(const std::array<double, 3>& plane1,
    const std::array<double, 3>& plane2,
    std::array<double, 3>& bary_coords)
{
    double n1 = plane1[2] * plane2[1] - plane1[1] * plane2[2];
    double n2 = plane1[0] * plane2[2] - plane1[2] * plane2[0];
    double n3 = plane1[1] * plane2[0] - plane1[0] * plane2[1];
    double d = n1 + n2 + n3;
    //
    bary_coords[0] = n1 / d;
    bary_coords[1] = n2 / d;
    bary_coords[2] = n3 / d;
}

// Point on tet edge
inline void compute_barycentric_coords(double f1, double f2, std::array<double, 2>& bary_coords)
{
    bary_coords[0] = f2 / (f2 - f1);
    bary_coords[1] = 1 - bary_coords[0];
}


// implicit functions
inline double sphere_function(
    const std::array<double, 3>& center, double r, const std::array<double, 3>& p)
{
    return (p[0] - center[0]) * (p[0] - center[0]) + (p[1] - center[1]) * (p[1] - center[1]) +
           (p[2] - center[2]) * (p[2] - center[2]) - r * r;
}

inline int sign(double x)
{
    return (x > 0) ? 1 : ((x < 0) ? -1 : 0);
}