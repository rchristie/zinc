/*******************************************************************************
FILE : computed_field.h

LAST MODIFIED : 12 October 2001

DESCRIPTION :
A Computed_field is an abstraction of an FE_field. For each FE_field there is
a wrapper Computed_field automatically generated that can be called on to
evaluate the field in an element or node. The interface for evaluating
Computed_fields is much simpler than for FE_field, since they hide details of
caching of FE_element_field_values, for example. Their main benefit is in
allowing new types of fields to be defined as functions of other fields and
source information, such as scale, offset, magnitude, gradient,
coordinate transformations etc., thus providing cmgui with the ability to
provide customised features without cluttering dialogs such as the graphical
element editor. Also, finite_element_to_graphics_object is greatly simplified
because it can assume all coordinate and other fields are in rectangular
cartesian coordinates - if they are not already, the rest of the program can
make a simple wrapper computed_field effecting this change. A positive
consequence of this change is that this file should in time contain the only
code for invoking coordinate transformations in the entire program.

In addition to these functional benefits, computed fields cache the last
position they were evaluated at and the values and derivatives of the field at
that point so that if values at that point are requested again they can be
immediately returned. This allows functions using a number of fields, some
possibly depending on each other to get the maximum benefit out of values that
have already been calculated, without requiring complex logic for determining
if a value is already known.
==============================================================================*/
#if !defined (COMPUTED_FIELD_H)
#define COMPUTED_FIELD_H

#include "command/parser.h"
#include "finite_element/finite_element.h"
#include "general/geometry.h"
#include "general/list.h"
#include "general/manager.h"
#include "general/object.h"
#include "user_interface/message.h"

/*
Global types
------------
*/

enum Computed_field_type
/******************************************************************************
LAST MODIFIED : 18 October 2000

DESCRIPTION :
==============================================================================*/
{
	COMPUTED_FIELD_INVALID,
	COMPUTED_FIELD_COMPONENT,          /* component of a Computed_field */
	COMPUTED_FIELD_COMPOSE,            /* compose three Computed_fields in sequence */
	COMPUTED_FIELD_CONSTANT,           /* constant N-component field */
	COMPUTED_FIELD_CUBIC_TEXTURE_COORDINATES, /* cube projected from a centre */
	COMPUTED_FIELD_EDIT_MASK,          /* edit particular components without affecting others */
	COMPUTED_FIELD_EXTERNAL,           /* uses an external program to perform computation */
	COMPUTED_FIELD_RC_COORDINATE,      /* converts from other coord systems */
	COMPUTED_FIELD_RC_VECTOR,          /* converts non-RC vector at coordinate */
	COMPUTED_FIELD_SUM_COMPONENTS,     /* weighted sum of field components */
	COMPUTED_FIELD_NEW_TYPES           /* all the new types to which all will be changed */
};

struct Computed_field;
/*******************************************************************************
LAST MODIFIED : 24 December 1998

DESCRIPTION :
struct Computed_field is private.
==============================================================================*/

DECLARE_LIST_TYPES(Computed_field);

DECLARE_MANAGER_TYPES(Computed_field);

struct Computed_field_package;
/*******************************************************************************
LAST MODIFIED : 3 February 1999

DESCRIPTION :
Contains all information for editing and maintaining Computed_fields, including
the MANAGER(Computed_field).
struct Computed_field_package is private.
???RC Make macro PACKAGE(Computed_field) etc.?
==============================================================================*/

struct Set_Computed_field_conditional_data
/*******************************************************************************
LAST MODIFIED : 3 February 1999

DESCRIPTION :
User data structure passed to set_Computed_field_conditional, containing the
computed_field_package and the optional conditional_function (and
conditional_function_user_data) for selecting a field out of a subset of the
fields in the manager.
==============================================================================*/
{
	MANAGER_CONDITIONAL_FUNCTION(Computed_field) *conditional_function;
	void *conditional_function_user_data;
	struct MANAGER(Computed_field) *computed_field_manager;
}; /* struct Set_Computed_field_conditional_data */

struct Set_Computed_field_array_data
/*******************************************************************************
LAST MODIFIED : 9 March 1999

DESCRIPTION :
User data structure passed to set_Computed_field_array, containing the number of
fields in the array, and a pointer to a Set_Computed_field_conditional_data
structure for passing to set_Computed_field_conditional - this is done for each
element in the array.
==============================================================================*/
{
	int number_of_fields;
	struct Set_Computed_field_conditional_data *conditional_data;
}; /* struct Set_Computed_field_array_data */

/*
Global functions
----------------
*/
struct Computed_field *CREATE(Computed_field)(char *name);
/*******************************************************************************
LAST MODIFIED : 11 March 1999

DESCRIPTION :
Creates a basic Computed_field with the given <name>. Its type is initially
COMPUTED_FIELD_CONSTANT with 1 component, returning a value of zero.
==============================================================================*/

int DESTROY(Computed_field)(struct Computed_field **field_address);
/*******************************************************************************
LAST MODIFIED : 24 December 1998

DESCRIPTION :
Frees memory/deaccess objects in computed_field at <*field_address>.
==============================================================================*/

PROTOTYPE_OBJECT_FUNCTIONS(Computed_field);
/*PROTOTYPE_COPY_OBJECT_FUNCTION(Computed_field);*/
PROTOTYPE_GET_OBJECT_NAME_FUNCTION(Computed_field);

PROTOTYPE_LIST_FUNCTIONS(Computed_field);
PROTOTYPE_FIND_BY_IDENTIFIER_IN_LIST_FUNCTION(Computed_field,name,char *);

PROTOTYPE_MANAGER_COPY_FUNCTIONS(Computed_field,name,char *);
PROTOTYPE_MANAGER_FUNCTIONS(Computed_field);
PROTOTYPE_MANAGER_IDENTIFIER_FUNCTIONS(Computed_field,name,char *);

int Computed_field_clear_cache(struct Computed_field *field);
/*******************************************************************************
LAST MODIFIED : 10 February 1999

DESCRIPTION :
Clears any caching of elements/values of <field> and any fields it refers to.
Must call this function for each field after you have used functions
Computed_field_evaluate_in_element or Computed_field_evaluate_at_node and they
are possibly not going to be called again for some time.
==============================================================================*/

int Computed_field_is_defined_in_element(struct Computed_field *field,
	struct FE_element *element);
/*******************************************************************************
LAST MODIFIED : 13 May 1999

DESCRIPTION :
Returns true if <field> can be calculated in <element>. If the field depends on
any other fields, this function is recursively called for them.
==============================================================================*/

int Computed_field_is_defined_in_element_conditional(
	struct Computed_field *field,void *element_void);
/*******************************************************************************
LAST MODIFIED : 23 May 2000

DESCRIPTION :
Manager conditional function version of Computed_field_is_defined_in_element.
==============================================================================*/

int Computed_field_is_defined_at_node(struct Computed_field *field,
	struct FE_node *node);
/*******************************************************************************
LAST MODIFIED : 13 May 1999

DESCRIPTION :
Returns true if <field> can be calculated at <node>. If the field depends on
any other fields, this function is recursively called for them.
==============================================================================*/

int Computed_field_is_defined_at_node_conditional(struct Computed_field *field,
	void *node_void);
/*******************************************************************************
LAST MODIFIED : 28 June 2000

DESCRIPTION :
Computed_field conditional function version of
Computed_field_is_defined_at_node.
==============================================================================*/

int FE_node_has_Computed_field_defined(struct FE_node *node,void *field_void);
/*******************************************************************************
LAST MODIFIED : 28 June 2000

DESCRIPTION :
FE_node conditional function version of Computed_field_is_defined_at_node.
==============================================================================*/

int Computed_field_depends_on_Computed_field(struct Computed_field *field,
	struct Computed_field *other_field);
/*******************************************************************************
LAST MODIFIED : 26 January 1999

DESCRIPTION :
Returns true if the two fields are identical or one of the source_fields of
<field> is <other_field> or depends on it.

This function is used by MANAGER_COPY_WITHOUT_IDENTIFIER to ensure that the
destination field will not depend on itself, thus leading to an infinite loop,
and also to prevent copying a field over itself.

Parts of the program receiving manager messages for Computed_fields should call
this function with the field=field in use and other_field=modified field to
determine if the field in use needs updating.
==============================================================================*/

int Computed_field_depends_on_Computed_field_in_list(
	struct Computed_field *field, struct LIST(Computed_field) *field_list);
/*******************************************************************************
LAST MODIFIED : 28 May 2001

DESCRIPTION :
Returns true if <field> depends on any field in <field_list>.
==============================================================================*/

char *Computed_field_evaluate_as_string_in_element(
	struct Computed_field *field,int component_number,
	struct FE_element *element,FE_value *xi,struct FE_element *top_level_element);
/*******************************************************************************
LAST MODIFIED : 30 June 2000

DESCRIPTION :
Returns a string representing the value of <field>.<component_number> at
<element>:<xi>. If <field> is a FINITE_ELEMENT wrapper without a value type of
FE_VALUE_VALUE, requests the string from it, otherwise calls
Computed_field_evaluate_cache_in_element and converts the value for
<component_number> to a string (since result may already be in cache).

Use -1 as the <component_number> if you want all the components.

The <top_level_element> parameter has the same use as in
Computed_field_evaluate_cache_in_element.

Some basic field types such as CMISS_NUMBER have special uses in this function.
It is up to the calling function to DEALLOCATE the returned string.
???RC.  Allow derivatives to be evaluated as string too?
==============================================================================*/

int Computed_field_evaluate_in_element(struct Computed_field *field,
	struct FE_element *element,FE_value *xi,struct FE_element *top_level_element,
	FE_value *values,FE_value *derivatives);
/*******************************************************************************
LAST MODIFIED : 29 June 1999

DESCRIPTION :
Returns the values and derivatives (if <derivatives> != NULL) of <field> at
<element>:<xi>, if it is defined over the element. Can verify this in advance
by calling function Computed_field_defined_in_element. Each <field> has a cache
for storing its values and derivatives, which is allocated and filled by a call
to Computed_field_evaluate_cache_in_element, then copied to the <values> and
<derivatives> arrays.

The optional <top_level_element> may be supplied for the benefit of this or
any source fields that may require calculation on it instead of a face or line.
FIBRE_AXES and GRADIENT are examples of such fields, since they require
top-level coordinate derivatives. The term "top_level" refers to an ultimate
parent element for the face or line, eg. the 3-D element parent to 2-D faces.
If no such top level element is supplied and one is required, then the first
available parent element will be chosen - if the user requires a top-level
element in the same group as the face or with the face on the correct side,
then they should supply the top_level_element here.

The <values> and <derivatives> arrays must be large enough to store all the
values and derivatives for this field in the given element, ie. values is
number_of_components in size, derivatives has the element dimension times the
number_of_components
==============================================================================*/

char *Computed_field_evaluate_as_string_at_node(struct Computed_field *field,
	int component_number, struct FE_node *node);
/*******************************************************************************
LAST MODIFIED : 3 July 2000

DESCRIPTION :
Returns a string describing the value/s of the <field> at the <node>. If the
field is based on an FE_field but not returning FE_values, it is asked to supply
the string. Otherwise, a string built up of comma separated values evaluated
for the field in Computed_field_evaluate_cache_at_node. The FE_value exception
is used since it is likely the values are already in the cache in most cases,
or can be used by other fields again if calculated now.
The <component_number> indicates which component to calculate.  Use -1 to 
create a string which represents all the components.
Some basic field types such as CMISS_NUMBER have special uses in this function.
It is up to the calling function to DEALLOCATE the returned string.
==============================================================================*/

int Computed_field_evaluate_at_node(struct Computed_field *field,
	struct FE_node *node,FE_value *values);
/*******************************************************************************
LAST MODIFIED : 10 February 1999

DESCRIPTION :
Returns the <values> of <field> at <node> if it is defined there. Can verify
this in advance by calling function Computed_field_defined_at_node. Each <field>
has a cache for storing its values and derivatives, which is allocated and the
field->values array filled by a call to Computed_field_evaluate_cache_at_node,
which is then copied to <values> by this function. Derivatives may only be
calculated in elements, however, the field->derivatives array is allocated here
with field->values since Computed_field_evaluate_cache_in_element expects both
to be allocated together.

The <values> array must be large enough to store as many FE_values as there are
number_of_components.
==============================================================================*/

int Computed_field_set_values_at_node(struct Computed_field *field,
	struct FE_node *node,FE_value *values);
/*******************************************************************************
LAST MODIFIED : 20 April 2000

DESCRIPTION :
Sets the <values> of the computed <field> at <node>. Only certain computed field
types allow their values to be set. Fields that deal directly with FE_fields eg.
FINITE_ELEMENT and NODE_VALUE fall into this category, as do the various
transformations, RC_COORDINATE, RC_VECTOR, OFFSET, SCALE, etc. which convert
the values into what they expect from their source field, and then call the same
function for it. If a field has more than one source field, eg. RC_VECTOR, it
can in many cases still choose which one is actually being changed, for example,
the 'vector' field in this case - coordinates should not change. This process
continues until the actual FE_field values at the node are changed or a field
is reached for which its calculation is not reversible, or is not supported yet.
Note that you must only call this function for nodes that are not managed as it
will change values inside them. Also, this function does not clear the cache at
any time, so up to the calling function to do so.
Note that the values array will not be modified by this function. Also, <node>
should not be managed at the time it is modified by this function.
???RC Note that some functions are not reversible in this way.
==============================================================================*/

int Computed_field_set_values_at_managed_node(struct Computed_field *field,
	struct FE_node *node,FE_value *values,struct MANAGER(FE_node) *node_manager);
/*******************************************************************************
LAST MODIFIED : 20 April 2000

DESCRIPTION :
Sets the <values> of the computed <field> at <node>. Only certain computed field
types allow their values to be set. Fields that deal directly with FE_fields eg.
FINITE_ELEMENT and NODE_VALUE fall into this category, as do the various
transformations, RC_COORDINATE, RC_VECTOR, OFFSET, SCALE, etc. which convert
the values into what they expect from their source field, and then call the same
function for it. If a field has more than one source field, eg. RC_VECTOR, it
can in many cases still choose which one is actually being changed, for example,
the 'vector' field in this case - coordinates should not change. This process
continues until the actual FE_field values at the node are changed or a field
is reached for which its calculation is not reversible, or is not supported yet.

This function works by making a copy of the node, then performing all
modifications to it. If these are successful then MANAGER_MODIFY_NOT_IDENTIFIER
is called to effect the change on the real <node>, thus sending manager messages
to the rest of the program. Because all changes are made on a temporary copy of
the node, all current cache values will be ignored. For safety, however, the
cache is always cleared after calling.
It is up to calling function to begin/end node manager cache if more than one
node is being modified.
Note that the values array will not be modified by this function.
==============================================================================*/

int Computed_field_get_values_in_element(struct Computed_field *field,
	struct FE_element *element,int *number_in_xi,FE_value **values);
/*******************************************************************************
LAST MODIFIED : 28 October 1999

DESCRIPTION :
Companion function to Computed_field_set_values_in_element.
Returns the <field> calculated at the corners of the <number_in_xi> cells,
evenly spaced in xi, over the element. <values> should be allocated with enough
space for number_of_components * product of number_in_xi+1 in each element
direction, the returned values cycling fastest through number of grid points in
xi1, number of grid points in xi2 etc. and lastly components.
It is up to the calling function to deallocate the returned values.
==============================================================================*/

int Computed_field_set_values_in_element(struct Computed_field *field,
	struct FE_element *element,int *number_in_xi,FE_value *values);
/*******************************************************************************
LAST MODIFIED : 20 April 2000

DESCRIPTION :
Sets the <values> of the computed <field> over the <element>. Only certain
computed field types allow their values to be set. Fields that deal directly
with FE_fields eg. FINITE_ELEMENT fall into this category, as do the various
transformations, RC_COORDINATE, RC_VECTOR, OFFSET, SCALE, etc. which convert
the values into what they expect from their source field, and then call the
same function for it. If a field has more than one source field, eg. RC_VECTOR,
it can in many cases still choose which one is actually being changed, for
example, the 'vector' field in this case - coordinates should not change. This
process continues until the actual FE_field values in the element are changed or
a field is reached for which its calculation is not reversible, or is not
supported yet.

<number_in_xi> has the number of grid cells in each xi direction of <element>,
such that there is one more grid point in each direction than this number. Grid
points are evenly spaced in xi. There are as many <values> as there are grid
points X number_of_components, cycling fastest through number of grid points in
xi1, number of grid points in xi2 etc. and lastly components.

Note that the values array will not be modified by this function. Also,
<element> should not be managed at the time it is modified by this function.
???RC Note that some functions are not reversible in this way.
==============================================================================*/

int Computed_field_set_values_in_managed_element(struct Computed_field *field,
	struct FE_element *element,int *number_in_xi,FE_value *values,
	struct MANAGER(FE_element) *element_manager);
/*******************************************************************************
LAST MODIFIED : 20 April 2000

DESCRIPTION :
Sets the <values> of the computed <field> over the <element>. Only certain
computed field types allow their values to be set. Fields that deal directly
with FE_fields eg. FINITE_ELEMENT fall into this category, as do the various
transformations, RC_COORDINATE, RC_VECTOR, OFFSET, SCALE, etc. which convert
the values into what they expect from their source field, and then call the
same function for it. If a field has more than one source field, eg. RC_VECTOR,
it can in many cases still choose which one is actually being changed, for
example, the 'vector' field in this case - coordinates should not change. This
process continues until the actual FE_field values in the element are changed or
a field is reached for which its calculation is not reversible, or is not
supported yet.

<number_in_xi> has the number of grid cells in each xi direction of <element>,
such that there is one more grid point in each direction than this number. Grid
points are evenly spaced in xi. There are as many <values> as there are grid
points X number_of_components, cycling fastest through number of grid points in
xi1, number of grid points in xi2 etc. and lastly components.

This function works by making a copy of the element, then performing all
modifications to it. If these are successful then MANAGER_MODIFY_NOT_IDENTIFIER
is called to effect the change on the real <element>, thus sending manager
messages to the rest of the program. Because all changes are made on a temporary
copy of the element, all current cache values will be ignored. For safety,
however, the cache is always cleared after calling.
It is up to calling function to begin/end node manager cache if more than one
node is being modified.
Note that the values array will not be modified by this function.
==============================================================================*/

int Computed_field_get_native_discretization_in_element(
	struct Computed_field *field,struct FE_element *element,int *number_in_xi);
/*******************************************************************************
LAST MODIFIED : 28 October 1999 

DESCRIPTION :
If the <field> or its source field is grid-based in <element>, returns in
<number_in_xi> the numbers of finite difference cells in each xi-direction of
<element>. Note that this number is one less than the number of grid points in
each direction. <number_in_xi> should be allocated with at least as much space
as the number of dimensions in <element>, but is assumed to have no more than
MAXIMUM_ELEMENT_XI_DIMENSIONS so that
int number_in_xi[MAXIMUM_ELEMENT_XI_DIMENSIONS] can be passed to this function.
Returns 0 with no errors if the field is not grid-based.
==============================================================================*/

int Computed_field_get_number_of_components(struct Computed_field *field);
/*******************************************************************************
LAST MODIFIED : 23 December 1998

DESCRIPTION :
==============================================================================*/

char *Computed_field_get_component_name(struct Computed_field *field,
	int component_no);
/*******************************************************************************
LAST MODIFIED : 29 December 1998

DESCRIPTION :
Returns an allocated string containing the name of <component_no> of <field>.
Name of the component depends on the type of the Computed_field; for example,
COMPUTED_FIELD_FINITE_ELEMENT gets component names from the FE_field it is wrapping. Default
names are made out of the character form of the component_no+1, eg, 1 -> "2".
It is up to the calling function to deallocate the returned string.
==============================================================================*/

struct Coordinate_system *Computed_field_get_coordinate_system(
	struct Computed_field *field);
/*******************************************************************************
LAST MODIFIED : 19 January 1999

DESCRIPTION :
Returns the coordinate system <field> is to be interpreted under. See function
Computed_field_set_coordinate_system for further details.
==============================================================================*/

int Computed_field_set_coordinate_system(struct Computed_field *field,
	struct Coordinate_system *coordinate_system);
/*******************************************************************************
LAST MODIFIED : 19 January 1999

DESCRIPTION :
Sets the coordinate system <field> is to be interpreted under. Note the careful
choice of words here: the coordinate system merely tells the rest of the program
what needs to be done to transform the field values into any other coordinate
system. It does not have to be "correct" for the values in the field, eg. you
can describe prolate spheroidal values as RC to "open out" the heart model.
<focus> must be given with prolate and oblate spheroidal coordinate systems.
==============================================================================*/

enum Computed_field_type Computed_field_get_type(struct Computed_field *field);
/*******************************************************************************
LAST MODIFIED : 24 December 1998

DESCRIPTION :
Returns the type of the computed <field> eg. COMPUTED_FIELD_FINITE_ELEMENT etc.
==============================================================================*/

char *Computed_field_get_type_string(struct Computed_field *field);
/*******************************************************************************
LAST MODIFIED : 4 July 2000

DESCRIPTION :
Returns the string which identifies the type.
==============================================================================*/

char *Computed_field_type_to_string(struct Computed_field *field);
/*******************************************************************************
LAST MODIFIED : 25 January 1999

DESCRIPTION :
Returns a pointer to a static string token for the given <field_type>.
The calling function must not deallocate the returned string.
==============================================================================*/

int Computed_field_has_3_components(struct Computed_field *field,
	void *dummy_void);
/*******************************************************************************
LAST MODIFIED : 10 March 1999

DESCRIPTION :
Conditional function returning true if <field> has exactly three components.
==============================================================================*/

int Computed_field_has_4_components(struct Computed_field *field,
	void *dummy_void);
/*******************************************************************************
LAST MODIFIED : 10 March 1999

DESCRIPTION :
Conditional function returning true if <field> has exactly four components.
==============================================================================*/

int Computed_field_has_up_to_4_numerical_components(
	struct Computed_field *field,void *dummy_void);
/*******************************************************************************
LAST MODIFIED : 23 May 2000

DESCRIPTION :
Conditional function returning true if <field> has up to 4 components and they
are numerical - useful for selecting vector/coordinate fields.
==============================================================================*/

int Computed_field_has_n_components(struct Computed_field *field,
	void *components_ptr_void);
/*******************************************************************************
LAST MODIFIED : 11 July 2000

DESCRIPTION :
Iterator/conditional function returning true if <field> has the same number of
components as that specified by <components_ptr_void>.
==============================================================================*/

int Computed_field_has_at_least_2_components(struct Computed_field *field,
	void *dummy_void);
/*******************************************************************************
LAST MODIFIED : 16 March 1999

DESCRIPTION :
Iterator/conditional function returning true if <field> has at least 2 components.
==============================================================================*/

int Computed_field_has_numerical_components(struct Computed_field *field,
	void *dummy_void);
/*******************************************************************************
LAST MODIFIED : 23 May 2000

DESCRIPTION :
Conditional function returning true if <field> returns numerical components.
Note that whether the numbers were integer, FE_value or double, they may be
returned as FE_value when evaluated.
==============================================================================*/

int Computed_field_is_scalar(struct Computed_field *field,void *dummy_void);
/*******************************************************************************
LAST MODIFIED : 23 May 2000

DESCRIPTION :
Conditional function returning true if <field> has 1 component and it is
numerical.
==============================================================================*/

int Computed_field_has_up_to_3_numerical_components(
	struct Computed_field *field,void *dummy_void);
/*******************************************************************************
LAST MODIFIED : 23 May 2000

DESCRIPTION :
Conditional function returning true if <field> has up to 3 components and they
are numerical - useful for selecting vector/coordinate fields.
==============================================================================*/

int Computed_field_has_up_to_4_numerical_components(
	struct Computed_field *field,void *dummy_void);
/*******************************************************************************
LAST MODIFIED : 23 May 2000

DESCRIPTION :
Conditional function returning true if <field> has up to 4 components and they
are numerical - useful for selecting vector/coordinate fields.
==============================================================================*/

int Computed_field_is_orientation_scale_capable(struct Computed_field *field,
	void *dummy_void);
/*******************************************************************************
LAST MODIFIED : 12 February 1999

DESCRIPTION :
Conditional function returning true if the field can be used to orient or scale
glyphs. Generally, this means it has 1,2,3,4,6 or 9 components, where:
1 = scalar (no vector, isotropic scaling).
2 = 1 2-D vector (2nd axis is normal in plane, 3rd is out of 2-D plane);
3 = 1 3-D vector (orthogonal 2nd and 3rd axes are arbitrarily chosen);
4 = 2 2-D vectors (3rd axis taken as out of 2-D plane);
6 = 2 3-D vectors (3rd axis found from cross product);
9 = 3 3-D vectors = complete definition of 3 axes.
???RC.  Include coordinate system in test?
==============================================================================*/

int Computed_field_is_stream_vector_capable(struct Computed_field *field,
	void *dummy_void);
/*******************************************************************************
LAST MODIFIED : 15 March 1999

DESCRIPTION :
Conditional function returning true if the field is suitable for 3-D streamline
tracking. This means it has either 3, 6 or 9 components (with 3 components per
vector), or has a FIBRE coordinate_system, meaning it can be wrapped to produce
9-component fibre_axes.
The number of components controls how the field is interpreted:
3 = 1 3-D vector (lateral direction and normal worked out from curl of field);
6 = 2 3-D vectors (2nd vector is lateral direction. Stream ribbon normal found
    from cross product);
9 = 3 3-D vectors (2nd vector is lateral direction; 3rd vector is stream ribbon
    normal).
==============================================================================*/

int Computed_field_get_type_component(struct Computed_field *field,
	struct Computed_field **source_field,int *component_no);
/*******************************************************************************
LAST MODIFIED : 29 December 1998

DESCRIPTION :
If the field is of type COMPUTED_FIELD_COMPONENT, the source_field/component_no
used by it are returned - otherwise an error is reported.
Use function Computed_field_get_type to determine the field type.
==============================================================================*/

int Computed_field_set_type_component(struct Computed_field *field,
	struct Computed_field *source_field,int component_no);
/*******************************************************************************
LAST MODIFIED : 10 March 1998

DESCRIPTION :
Converts <field> to type COMPUTED_FIELD_COMPONENT, returning the value of
component <component_no> of <source_field>. Sets number of components to 1.
coordinate field to return a vector (eg. velocity from the potential scalar).
If function fails, field is guaranteed to be unchanged from its original state,
although its cache may be lost.
==============================================================================*/

int Computed_field_get_type_constant(struct Computed_field *field,
	int *number_of_values,FE_value **values);
/*******************************************************************************
LAST MODIFIED : 11 March 1999

DESCRIPTION :
If the field is of type COMPUTED_FIELD_CONSTANT, the number_of_values and
values it contains are returned. The <*values> array is allocated and returned
by this function - it is up to the calling function to DEALLOCATE it.
Use function Computed_field_get_type to determine the field type.
==============================================================================*/

int Computed_field_set_type_constant(struct Computed_field *field,
	int number_of_values,FE_value *values);
/*******************************************************************************
LAST MODIFIED : 11 March 1999

DESCRIPTION :
Converts <field> to type COMPUTED_FIELD_CONSTANT, returning the given array of
<values> everywhere.
Gives the field as many components as the specified <number_of_values>.
If function fails, field is guaranteed to be unchanged from its original state,
although its cache may be lost.
==============================================================================*/

int Computed_field_get_type_rc_coordinate(struct Computed_field *field,
	struct Computed_field **coordinate_field);
/*******************************************************************************
LAST MODIFIED : 19 January 1999

DESCRIPTION :
If the field is of type COMPUTED_FIELD_RC_COORDINATE, the coordinate field used
by it is returned - otherwise an error is reported.
Use function Computed_field_get_type to determine the field type.
==============================================================================*/

int Computed_field_set_type_rc_coordinate(struct Computed_field *field,
	struct Computed_field *coordinate_field);
/*******************************************************************************
LAST MODIFIED : 11 March 1999

DESCRIPTION :
Converts <field> to type COMPUTED_FIELD_RC_COORDINATE, which returns the values
and derivatives of the coordinate field in rectangular cartesian coordinates.
Coordinate_system of this type of field need not be RC, although it usually will
be. The <coordinate_field> must have no more than 3 components.
Sets the number of components to 3.
If function fails, field is guaranteed to be unchanged from its original state,
although its cache may be lost.
==============================================================================*/

int Computed_field_get_type_rc_vector(struct Computed_field *field,
	struct Computed_field **vector_field,
	struct Computed_field **coordinate_field);
/*******************************************************************************
LAST MODIFIED : 24 June 1999

DESCRIPTION :
If the field is of type COMPUTED_FIELD_RC_VECTOR, the vector and coordinate
fields used by it are returned - otherwise an error is reported.
Use function Computed_field_get_type to determine the field type.
==============================================================================*/

int Computed_field_set_type_rc_vector(struct Computed_field *field,
	struct Computed_field *vector_field,struct Computed_field *coordinate_field);
/*******************************************************************************
LAST MODIFIED : 24 June 1999

DESCRIPTION :
Converts <field> to type COMPUTED_FIELD_RC_VECTOR, combining a vector field
supplying a single vector (1,2 or 3 components), two vectors (4 or 6 components)
or three vectors (9 components) with a coordinate field. This field type ensures
that each source vector is converted to RC coordinates at the position given by
the coordinate field - as opposed to RC_COORDINATE which assumes the
transformation is always based at the origin.
Sets the number of components to 3 times the number of vectors expected from
the source vector_field.
If function fails, field is guaranteed to be unchanged from its original state,
although its cache may be lost.
==============================================================================*/

int Computed_field_get_type_sum_components(struct Computed_field *field,
	struct Computed_field **source_field,FE_value **weights);
/*******************************************************************************
LAST MODIFIED : 8 March 1999

DESCRIPTION :
If the field is of type COMPUTED_FIELD_SUM_COMPONENTS, the source_field and
weights used by it are returned. Since the number of weights is equal to the
number of components in the source_field (and you don't know this yet), this
function returns in *weights a pointer to an allocated array containing the
FE_values.
It is up to the calling function to DEALLOCATE the returned <*weights>.
Use function Computed_field_get_type to determine the field type.
==============================================================================*/

int Computed_field_set_type_sum_components(struct Computed_field *field,
	struct Computed_field *source_field,FE_value *weights);
/*******************************************************************************
LAST MODIFIED : 11 March 1999

DESCRIPTION :
Converts <field> to type COMPUTED_FIELD_SUM_COMPONENTS, returning a scalar
weighted sum of the components of <source_field>. The <weights> array must
contain as many FE_value weights as there are components in <source_field>.
Sets the number of components to 1.
If function fails, field is guaranteed to be unchanged from its original state,
although its cache may be lost.
==============================================================================*/

int Computed_field_set_type_xi_coordinates(struct Computed_field *field);
/*******************************************************************************
LAST MODIFIED : 11 March 1999

DESCRIPTION :
Converts <field> to type COMPUTED_FIELD_XI, defined only over elements and
returning the xi that is passed to it.
Sets the number of components to 3.
If function fails, field is guaranteed to be unchanged from its original state,
although its cache may be lost.
???RC May want the number of components to be user-selectable; for now, stick
with 3.
==============================================================================*/

int Computed_field_is_in_use(struct Computed_field *field);
/*******************************************************************************
LAST MODIFIED : 26 January 1999

DESCRIPTION :
Returns true if the field is accessed more than once; ie. it is in use somewhere
else in the program (apart from being accessed by its manager).
==============================================================================*/

int Computed_field_is_read_only(struct Computed_field *field);
/*******************************************************************************
LAST MODIFIED : 26 January 1999

DESCRIPTION :
Returns true if the field is read_only - use this to check if you are allowed
to modify or remove it from the manager.
==============================================================================*/

int Computed_field_set_read_only(struct Computed_field *field);
/*******************************************************************************
LAST MODIFIED : 26 January 1999

DESCRIPTION :
Marks <field> as read-only, telling the program that the user is not permitted
to modify or destroy it.
==============================================================================*/

int Computed_field_set_read_write(struct Computed_field *field);
/*******************************************************************************
LAST MODIFIED : 26 January 1999

DESCRIPTION :
Clears read-only status of <field>, telling the program that the user is allowed
to modify and destroy it.
==============================================================================*/

int Computed_field_find_element_xi(struct Computed_field *field,
	FE_value *values, int number_of_values, struct FE_element **element, 
	FE_value *xi, struct GROUP(FE_element) *search_element_group);
/*******************************************************************************
LAST MODIFIED : 17 July 2000

DESCRIPTION :
This function implements the reverse of some certain computed_fields
(Computed_field_is_find_element_xi_capable) so that it tries to find an element
and xi which would evaluate to the given values.
This has been implemented so that the texture_coordinates can be used to extract
information from textures (sample_texture computed_field) and then modified and
then put back into another texture.
The <search_element_group> is the set of elements from which the chosen element
will belong.
==============================================================================*/

int Computed_field_is_find_element_xi_capable(struct Computed_field *field,
	void *dummy_void);
/*******************************************************************************
LAST MODIFIED : 16 June 2000

DESCRIPTION :
This function returns true if the <field> can find element and xi given
a set of values.
==============================================================================*/

int define_Computed_field(struct Parse_state *state,void *field_copy_void,
	void *define_field_package_void);
/*******************************************************************************
LAST MODIFIED : 3 February 1999

DESCRIPTION :
Modifier entry function for creating and modifying Computed_fields. Format for
parameters from the parse state are:
  FIELD_NAME|NEW_FIELD_NAME
    components #
      rectangular_cartesian/cylindrical_polar/spherical_polar/prolate_sph...
        component
          FIELD_NAME.COMPONENT_NAME
        composite
				  scalars FIELD_NAME FIELD_NAME... FIELD_NAME{number_of_components}
        gradient
				  scalar FIELD_NAME
					coordinate FIELD_NAME
        rc_coordinate
					coordinate FIELD_NAME
        scale
				  field FIELD_NAME
				  values # # ... #{number_of_components}
        ... (more as more types added)
Note that the above layout is used because:
1. The number_of_components is often prerequisite information for setting up
the modifier functions for certain types of computed field, eg. "composite"
requires as many scalar fields as there are components, while scale has as many
floats.
2. The number_of_components and coordinate system are options for all types of
computed field so it makes sense that they are set before splitting up into the
options for the various types.
The <field_copy_void> parameter, if set, points to the field we are to modify
and should not itself be managed.
==============================================================================*/

int equivalent_computed_fields_at_elements(struct FE_element *element_1,
	struct FE_element *element_2);
/*******************************************************************************
LAST MODIFIED : 23 May 2000

DESCRIPTION :
Returns true if all fields are defined in the same way at the two elements.
==============================================================================*/

int equivalent_computed_fields_at_nodes(struct FE_node *node_1,
	struct FE_node *node_2);
/*******************************************************************************
LAST MODIFIED : 23 May 2000

DESCRIPTION :
Returns true if all fields are defined in the same way at the two nodes.
==============================================================================*/

struct Computed_field *Computed_field_manager_get_component_wrapper(
	struct MANAGER(Computed_field) *computed_field_manager,
	struct Computed_field *field,int component_no);
/*******************************************************************************
LAST MODIFIED : 3 December 1999

DESCRIPTION :
If a COMPONENT wrapper for <field> <component_no> exists in the
<computed_field_manager>, it is returned, otherwise a new one is made in the
manager and returned.
==============================================================================*/

int set_Computed_field_conditional(struct Parse_state *state,
	void *field_address_void,void *set_field_data_void);
/*******************************************************************************
LAST MODIFIED : 3 February 1999

DESCRIPTION :
Modifier function to set the field from a command. <set_field_data_void> should
point to a struct Set_Computed_field_conditional_data containing the
computed_field_package and an optional conditional function for narrowing the
range of fields available for selection. If the conditional_function is NULL,
this function works just like set_Computed_field.
==============================================================================*/

int set_Computed_field_array(struct Parse_state *state,
	void *field_array_void,void *set_field_array_data_void);
/*******************************************************************************
LAST MODIFIED : 9 March 1999

DESCRIPTION :
Modifier function to set an array of field from a command.
<set_field_array_data_void> should point to a struct
Set_Computed_field_array_conditional_data containing the number_of_fields in the
array, the computed_field_package and an optional conditional function for
narrowing the range of fields available for selection.
Works by repeatedly calling set_Computed_field_conditional.
???RC Make this globally available for calling any modifier function?
==============================================================================*/

int list_Computed_field(struct Computed_field *field,void *dummy_void);
/*******************************************************************************
LAST MODIFIED : 25 January 1999

DESCRIPTION :
Writes the properties of the <field> to the command window.
==============================================================================*/

int list_Computed_field_commands(struct Computed_field *field,
	void *command_prefix_void);
/*******************************************************************************
LAST MODIFIED : 25 January 1999

DESCRIPTION :
Writes the properties of the <field> to the command window.
==============================================================================*/

int Computed_field_commands_ready_to_list(struct Computed_field *field,
	void *list_of_fields_void);
/*******************************************************************************
LAST MODIFIED : 12 March 1999

DESCRIPTION :
Returns true if <field> is not in <list_of_fields>, but it has no source fields
or all are in the list. Used to ensure field commands are listed in the order
they must be created (so not referring to a field that is not created yet).
==============================================================================*/

int list_Computed_field_name(struct Computed_field *field,void *dummy_void);
/*******************************************************************************
LAST MODIFIED : 4 February 1999

DESCRIPTION :
Lists a single line about a computed field including just name, number of
components, coordinate system and type.
==============================================================================*/

struct Computed_field_package *CREATE(Computed_field_package)(
	struct MANAGER(FE_field) *fe_field_manager,
	struct MANAGER(FE_element) *fe_element_manager);
/*******************************************************************************
LAST MODIFIED : 21 May 2001

DESCRIPTION :
Creates a Computed_field_package which is used by the rest of the program to
access everything to do with computed fields. The computed_field_manager is
created here, and callbacks for changes in the fe_field_manager are established
so that wrapper Computed_fields are automatically created and updated to match
FE_fields.
==============================================================================*/

int DESTROY(Computed_field_package)(
	struct Computed_field_package **package_address);
/*******************************************************************************
LAST MODIFIED : 3 February 1999

DESCRIPTION :
Frees memory/deaccess objects in computed_field_package at <*package_address>.
Cancels any further messages from the MANAGER(FE_field).
==============================================================================*/

struct MANAGER(Computed_field)
	*Computed_field_package_get_computed_field_manager(
		struct Computed_field_package *computed_field_package);
/*******************************************************************************
LAST MODIFIED : 3 February 1999

DESCRIPTION :
Extracts the computed_field_manager from the computed_field_package. Note that
the rest of the program should use this sparingly - it is really only here to
allow interfacing to the choose_object widgets.
==============================================================================*/

int Computed_field_can_be_destroyed(struct Computed_field *field);
/*******************************************************************************
LAST MODIFIED : 10 May 2000

DESCRIPTION :
Returns true if the <field> is only accessed once - assumed by its manager. If
it is of type COMPUTED_FIELD_FINITE_ELEMENT further tests that its fe_field can
be destroyed, assuming it is only accessed by this field and its manager.
==============================================================================*/

#endif /* !defined (COMPUTED_FIELD_H) */
