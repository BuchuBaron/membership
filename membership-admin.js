Drupal.behaviors.membership_admin = function(context) {
  // Set up click events on the display selector checkboxes.
  $.each(['status', 'identity', 'contact', 'emergency'], function(index, value) {
    $('#' + value, context).click(function() {
      if ($(this).attr('checked')) {
        $('.' + value).show();
      }
      else {
        $('.' + value).hide();
      }
    });
    $('.status').hide();
    $('.emergency').hide();
    $('#contact').attr('checked', 'checked');
    $('#identity').attr('checked', 'checked');
  });

  // Set up select-all
  $('#select-all', context).click(function() {
    if ($(this).attr('checked')) {
      $('.selection').attr('checked', 'checked');
    }
    else {
      $('.selection').removeAttr('checked');
    }
  });

  // Set up unselect all when one checkbox is unselected.
  $('.selection', context).click(function() {
    if (!$(this).attr('checked')) {
      $('#select-all').removeAttr('checked');
    }
  });

  // Set up the actions on selected records.
  $('#run-action', context).click(function() {
    var command = $('#action').val();
    // Assemble a list of ids to send to the server with the command:
    var selected_members = $.map($('.selection:checked', context), function(element) {
      return element.value;
    });
    alert("Running " + command + " on selection of " + selected_members.join(','));
    var post_data = {
      // filter: filter,
      selected_members: selected_members
    };

    $.post('/membership/update/' + command, post_data, function(result) {
      if (result === 'no landingpages') {
        alert('Really?!');
      }
      else {
        alert("Ran " + command + " on " + selected_members.join(','));
      }
    });
  });
}
